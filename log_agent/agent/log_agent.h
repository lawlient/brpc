#ifndef ASYNCLOG_AGENT_HEADER__
#define ASYNCLOG_AGENT_HEADER__


#include "asynclog.h"
#include "async_logger.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



#define LOCKFILE "/var/run/asynclog.pid"


extern log_err_t AsyncLog_logger(AsyncLog *this, LogItem *log);

extern void AsyncLog_module_check(AsyncLog *this);

/* 子进程消费共享内存循环队列中的日志记录 */
extern void log_consumer(AsyncLog *);









#endif
