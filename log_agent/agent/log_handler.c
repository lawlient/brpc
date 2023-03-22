#include "log_agent.h"

#define BUFSIZE 256

typedef struct ModuleCache_ {
    int fd;
    ino_t inode;
} ModuleCache;

ModuleCache mcaches[MODUSIZE];

int fds[MODUSIZE] = {0};


static log_err_t check_file(AsyncLog *this, int id);



typedef log_err_t (*Handle)(AsyncLog *this, LogItem *log);

static log_err_t handle_idle(AsyncLog *this, LogItem *log);
static log_err_t handle_register(AsyncLog *this, LogItem *log);
static log_err_t handle_log(AsyncLog *this, LogItem *log);


static const Handle handlers[] = {
    handle_idle,
    handle_register,
    handle_log,
    NULL,
};

log_err_t AsyncLog_logger(AsyncLog *this, LogItem *log) {
    if (log->flag != READABLE)
        return E_LOG_WRITING;
    lbarrier();
    if (!Log_magic_valid(log)) 
        return E_LOG_MAGIC_ERR;
    if (!Log_type_valid(log->type)) 
        return E_INVALID_LOGTYPE;

    return handlers[log->type](this, log);
}

log_err_t handle_idle(AsyncLog *this, LogItem *log) { return E_OK; }

log_err_t handle_register(AsyncLog *this, LogItem *log) {
    if (log->len != sizeof(Item)) 
        return E_QUEUE_BROKEN;

    const Item *ritem = (Item *)log->data;
    Item *item = &this->module.item[log->id];
    memcpy(item, ritem, sizeof(Item));

    State *stat = &this->module.stat[log->id];
    stat->registered = 1;
    return 0;
}

log_err_t handle_log(AsyncLog *this, LogItem *log) {
    if (!Module_id_valid(log->id)) 
        return E_MODULE_ID_ERR;

    log_err_t err = check_file(this, log->id);
    if (err) 
        return err;

    ModuleCache *mc = &mcaches[log->id];
    write(mc->fd, log->data, log->len);

    return E_OK;
}



log_err_t check_file(AsyncLog *this, int id) {
    ModuleCache *mc = &mcaches[id];
    Item *item      = &this->module.item[id];
    State *state    = &this->module.stat[id];

    char filename[BUFSIZE];
    AsyncLog_filename(this, id, filename, BUFSIZE);

    struct stat st;
    if (!stat(filename, &st) && st.st_ino == mc->inode) {
        if (st.st_size >= item->max) {
            if (!state->full) {
                dprintf(mc->fd, "size of the log file is huge than %d\n", item->max);
                state->full = 1;
            }
            return E_LOG_FILE_FULL;
        }
        return E_OK;
    }

    if (mc->fd) {
        close(mc->fd);
        mc->fd = -1;
    }

    mc->fd = open(filename, O_APPEND | O_CREAT | O_RDWR, 0666);
    if (mc->fd == -1 && errno == ENOENT) {
        int err = mkdir(dirname(filename), 0755); 
        if (err)
            return E_CREAT_LOGDIR_FAIL;

        mc->fd = open(filename, O_APPEND | O_CREAT | O_RDWR, 0666);
        if (mc->fd == -1)
            return E_OPEN_LOG_FILE_FAIL;
    }

    if (!fstat(mc->fd, &st))
        mc->inode = st.st_ino;

    return E_OK;
}

