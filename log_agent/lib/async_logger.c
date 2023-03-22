#include "async_logger.h"

#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <stdlib.h>

#include <assert.h>


static void Log_attach(Log *this);
static int Log_prefix(Severity severity, const char* file, int line, const char* func, char buf[], size_t len);

static const char *kSeverityName[] = { "debug  ", "info   ", "warning", "error  ", NULL, };


Log *Log_new(Severity severity, int id, int max, const char* path) {
    assert(path);
    Log *log = malloc(sizeof(Log));
    if (NULL == log) return NULL;
    log->severity = severity;
    log->id       = id;
    log->max      = max;
    strncpy((char *)log->path, path, PATH_LEN);
    log->shmm = NULL;
    return log;
}

log_err_t Log_register(Log *this) {
    log_err_t err = E_OK;
    Log_attach(this);
    AsyncLog *alog = this->shmm;

    LogItem *log = NULL; 
    err = AsyncLog_enqueue(alog, sizeof(Item), &log);
    if (err) {
        return err;
    }

    log->flag    = WRITING;
    log->version = 0;
    log->magic   = LOG_MAGIC;
    log->type    = REGISTER;
    log->len     = sizeof(Item);
    log->id      = this->id;

    Item *item  = (Item *)log->data;
    strncpy(item->path, this->path, PATH_LEN);
    item->max   = this->max;
    item->level = this->severity;

    sbarrier();
    log->flag = READABLE;
    return E_OK;
}


log_err_t Log_log(Log *this, Severity severity, const char* file, int line, const char* func, const char *fmt, ...) {
    if (!Log_severity_valid(severity)) 
        return E_INVALID_LEVEL;

    if (this->shmm == NULL) {
        Log_attach(this);
        if (this->shmm == NULL) {
            return E_NOT_ATTACH;
        }
    }
    AsyncLog *alog = this->shmm;

    if (alog->header.reset) 
        return E_SHMM_RESETING;
    if (alog->module.stat[this->id].registered == 0) 
        return E_MODULE_NOT_REG;
    if (alog->module.stat[this->id].full)
        return E_MODULE_FULL;


    char buf[LOG_LINE_LEN];
    uint16_t size = 0;
    size += Log_prefix(severity, file, line, func, buf, LOG_LINE_LEN-1);

    va_list ap;
    va_start(ap, fmt);
    size += vsnprintf(buf+size, LOG_LINE_LEN-1-size, fmt, ap);
    va_end(ap);

    buf[size++] = '\n';  // append a cr

    LogItem *log = NULL;
    log_err_t err = AsyncLog_enqueue(alog, size, &log);
    if (err) {
        return err;
    }

    log->flag    = WRITING;
    log->version = 0;
    log->magic   = LOG_MAGIC;
    log->type    = LOG;
    log->id      = this->id;
    log->len     = size;
    memcpy(log->data, buf, log->len);

    sbarrier();
    log->flag = READABLE;
    return E_OK;
}


void Log_attach(Log *this) { if (!this->shmm) this->shmm = AsyncLog_attach(); }


/* return : writen size */
int Log_prefix(Severity severity, const char* file, int line, const char* func,
               char buf[], size_t len) {
    int size = 0; /* used size */

    /* time */
    time_t now = time(0);
    struct tm tm;
    localtime_r(&now, &tm);
    size += strftime(buf, len, "%F %T", &tm);

    struct timeval msec;
    gettimeofday(&msec, NULL);
    size += snprintf(buf + size, len - size, ":%6ld ", msec.tv_usec);

    /* level */
    if (Log_severity_valid(severity)) {
        size += snprintf(buf + size, len - size, "%s ", kSeverityName[severity]);
    }

    /* pid */
    size += snprintf(buf + size, len - size, " %d", getpid());

    /* filename:line [funcname] */
    size += snprintf(buf + size, len - size, " %s:%d [%s] ", file, line, func);
    return size;
}

