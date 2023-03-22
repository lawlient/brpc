#include "log_agent.h"




void log_consumer(AsyncLog *alog) {
    do {
        LogItem *log = AsyncLog_peekqueue(alog);
        if (NULL == log /* empty */) {
            AsyncLog_module_check(alog);
            usleep(1 * 1000);
            continue;
        }

        AsyncLog_logger(alog, log);

        AsyncLog_dequeue(alog);

        AsyncLog_module_check(alog);

    } while (1);

}




void AsyncLog_module_check(AsyncLog *this) {
    for (int id = 0; id <= MODUSIZE; id++) {
        State * s = &this->module.stat[id];
        if (!s->registered) continue;

        if (!s->full) continue;

        /* check full state */
        char filename[PATH_LEN];
        AsyncLog_filename(this, id, filename, PATH_LEN);

        struct stat st;
        if (-1 == stat(filename, &st))
            s->full = 0;

        Item *item = &this->module.item[id];
        if (st.st_size < item->max)
            s->full = 0;
    }
}
