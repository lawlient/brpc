#include "log_agent.h"

#ifdef CONSOLE
#include <console.h>
#endif

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>



static AsyncLog *glog = NULL;

static void try_lock_file();
static void run_consumer();
static void guard_consumer();



#ifdef CONSOLE
static int show(int clientfd, int argc, char *argv[]);

static const Command cmds[] = {
    {"show", show},
    {0, 0},
};
#endif

int main(int argc, char *argv[]) {
#ifdef DAEMON
    if (daemon(0, 0) < 0) {
        perror("daemonize failure");
        exit(errno);
    }
#endif

    try_lock_file(LOCKFILE);

    run_consumer();

    guard_consumer();

    return 0;
}





void try_lock_file(const char *lockfile) {
    assert(lockfile);
    int fd = open(lockfile, O_WRONLY | O_CREAT, 0666);
    if (fd < 0) {
        perror("open lockfile fail");
        exit(errno);
    }

    fchmod(fd, 0666);
    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;

    if (fcntl(fd, F_SETLK, &lock) < 0) {
        perror(strerror(errno));
        exit(errno);
    }

    dprintf(fd, "%d\n", getpid());
    return;
}

void run_consumer() {
    int cid = fork();
    if (cid < 0) {

    } else if (cid == 0) {
        glog = AsyncLog_new();
        if (NULL == glog) exit(0);

#ifdef CONSOLE
        Console console;
        Console_init(&console, 7896, cmds);
#endif

        log_consumer(glog);
        sleep(2);
        exit(0);
    } else {
        // syslog start consumer success
    }
}

void guard_consumer() {
    FILE *log = fopen("/home/xujiuan/lawlient/notes/log/log", "w+");
    int cid, st;
    for (;;) {
        while ((cid = waitpid(-1, &st, WNOHANG)) > 0) {
            if (log) {
                if (WIFEXITED(st)) {
                    fprintf(log, "exited, status=%d\n", WEXITSTATUS(st));
                } else if (WIFSIGNALED(st)) {
                    fprintf(log, "killed by signal %d\n", WTERMSIG(st));
                } else if (WIFSTOPPED(st)) {
                    fprintf(log, "stopped by signal %d\n", WSTOPSIG(st));
                } else if (WIFCONTINUED(st)) {
                    fprintf(log, "continued\n");
                } else if (WCOREDUMP(st)) {
                    fprintf(log, "coredump\n");
                }
                fclose(log);
                return;
            }
            run_consumer();
        }
        usleep(1000);
    }
}


#ifdef CONSOLE
int show(int clientfd, int argc, char *argv[]) {
    if (glog == NULL) {
        dprintf(clientfd, "cannot access to shared memory\n");
        return 0;
    }

    dprintf(clientfd, "head: %d\n", glog->header.head);
    dprintf(clientfd, "tail: %d\n", glog->header.tail);
    dprintf(clientfd, "length: %d\n", glog->header.len);
    return 0;
}
#endif
