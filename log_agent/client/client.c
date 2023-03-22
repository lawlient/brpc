#include "asynclog.h"

#include "async_logger.h"

#include <pthread.h>

#define KB (1 << 10)
#define MB (1 << 20)

#define CONCURRENCY 4


static Log *glog = NULL;

static void *producer(void *i);


int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Usage: %s id module_path max\n", argv[0]);
        return 0;
    }

    int id             = atoi(argv[1]);
    const char* module = argv[2];
    int max            = atoi(argv[3]);

    glog = Log_new(0, id, max * KB, module);
    if (!glog) {
        printf("init log fail\n");
        return 0;
    }

    log_err_t err = Log_register(glog);
    if (err) {
        return err;
    }

    pthread_t ids[CONCURRENCY];
    int args[CONCURRENCY];
    for (int i = 0; i < CONCURRENCY; i++) {
        args[i] = i;
        pthread_create(&ids[i], NULL, producer, args + i);
    }

    for (int i = 0; i < CONCURRENCY; i++) {
        pthread_join(ids[i], NULL);
    }

    return 0;
}



void *producer(void *i) {
    int idx       = *(int*)i;
    log_err_t err = E_OK;
    Severity level     = Info;
    switch (idx) {
        case 0: level = Debug; break;
        case 1: level = Info; break;
        case 2: level = Warning; break;
        case 3: level = Error; break;
    }

    do {

        err = LOG(glog, level, "this is a log from thread %ld, module id: %d, path: %s", pthread_self(), glog->id, glog->path);

        switch (err) {
        case E_OK: usleep(500 * 1000);
        case E_MODULE_NOT_REG: {
           err = Log_register(glog);
           break;
        }
        default:
           sleep(1); 
        }

    } while (1);

    return 0;
}
