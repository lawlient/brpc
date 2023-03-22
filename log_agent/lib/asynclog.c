#include "asynclog.h"


#include <sys/ipc.h>
#include <sys/shm.h>

#include <errno.h>
#include <stdio.h>

static AsyncLog *AsyncLog_reset(AsyncLog *this);

static const LogItem kLogItem = {
    .flag     = READABLE,
    .version  = 0,
    .magic    = LOG_MAGIC,
    .type     = IDLE,       /* do nothing   */
    .id       = MODUSIZE,   /* out of range */
    .len      = 0,          /* no data      */
};


AsyncLog *AsyncLog_new() {
    int shmmid = shmget(SHMM_KEY, SHMM_SIZE, IPC_EXCL | IPC_CREAT | 0666);
    if (shmmid == -1 && errno == EEXIST) {
        shmmid = shmget(SHMM_KEY, 0, IPC_CREAT | 0666);
    }
    if (shmmid == -1) return NULL;

    void *shmm = shmat(shmmid, NULL, 0);
    if (shmm == (void *)-1) {
        perror(" shmat fail");
        exit(errno);
    }

    return AsyncLog_reset(shmm);
}

AsyncLog *AsyncLog_reset(AsyncLog *this) {
    this->header.magic = SHMM_MAGIC;
    this->header.head  = 0;
    this->header.tail  = 0;
    this->header.len   = (char*)this + SHMM_SIZE - this->body;
    this->header.len  -= sizeof(LogItem); /* protect region */
    this->header.reset = 0;
    return this;
}

AsyncLog *AsyncLog_attach() {
    int shmmid = shmget(SHMM_KEY, 0, IPC_CREAT | 0666);
    if (-1 == shmmid) return NULL;

    void *shmm = shmat(shmmid, NULL, 0);
    if (shmm == (void *)-1) return NULL;

    AsyncLog *alog = shmm;
    return alog;
}



int AsyncLog_delete() {
    return 0;
}

log_err_t AsyncLog_enqueue(AsyncLog *this, int len, LogItem **log) {
    int try = 0;
    int hsize = sizeof(LogItem);
    int ctail = 0, ntail = 0;
    do {
        if (try++ > 10) {
            *log = NULL;
            return E_CAS_FAIL;
        }
        ctail = this->header.tail;
        ntail = ctail + hsize + len;
        if (ntail > this->header.len) {
            /* reach protect region */
            if (this->header.head < hsize + len) {
                /* space from body is not satisfied supply */
                *log = NULL;
                return E_QUEUE_FULL;
            }

            /* start from body again */
            ntail = hsize + len;
        }
    } while (!CAS(this->header.tail, ctail, ntail));

    if (ntail == hsize + len) {
        /* set a default LogItem with 0 len */
        memcpy(this->body + ctail, &kLogItem, sizeof(kLogItem));
        *log = (LogItem *)(this->body);
        return E_OK;
    }

    *log = (LogItem *)(this->body + ctail);
    return E_OK;
}

LogItem *AsyncLog_peekqueue(AsyncLog *this) {
    if (AsyncLog_empty(this)) {
        return NULL;
    }
    LogItem *log = (LogItem *)(this->body + this->header.head);
    return log;
}

void AsyncLog_dequeue(AsyncLog *this) {
    if (AsyncLog_empty(this)) return;
    LogItem *log = (LogItem *)(this->body + this->header.head);
    this->header.head += sizeof(LogItem) + log->len;
    if (this->header.head >= this->header.len) {
        this->header.head = 0;
    }
}

/* 获取module当前对应的日志文件绝对路径名
 * 返回：名字长度 */
int AsyncLog_filename(AsyncLog *this, int id, char buf[], int len) {
    Item *item   = &this->module.item[id];
    int size = 0;
    size += snprintf(buf, len, LOG_PREFIX"%s/", item->path);
    time_t now = time(0);
    struct tm tm;
    localtime_r(&now, &tm);
    size += strftime(buf+size, len-size, "%F-%H.log", &tm);
    return size;
}

