#include "err.h"
#include "utils.h"
#include <pthread.h>
#include <signal.h>
#include <stdatomic.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MILI_TO_MICRO 1000

#define MAX_OUT_LENGTH 1022
#define MAX_N_TASKS 4096
static const size_t MAX_IN_LENGTH = 511;

static const char *RUN = "run";
static const char *SLEEP = "sleep";
static const char *ERR = "err";
static const char *OUT = "out";
static const char *KILL = "kill";
static const char *QUIT = "quit";

static const size_t SPACE_LENGTH = 1;
static const size_t RUN_LENGTH = 3;
static const size_t SLEEP_LENGTH = 5;
static const size_t ERR_LENGTH = 3;
static const size_t OUT_LENGTH = 3;
static const size_t KILL_LENGTH = 4;
static const size_t QUIT_LENGTH = 4;

typedef struct {
    size_t id;
    pthread_mutex_t mutexOut;
    pthread_mutex_t mutexErr;
    pid_t runnerPid;
    pthread_t outWriter;
    pthread_t errWriter;
    int outDsc;
    int errDsc;
    char out[MAX_OUT_LENGTH];
    char err[MAX_OUT_LENGTH];
} Task;

static Task tasks[MAX_N_TASKS];
atomic_int waitingToFinish;
static pthread_mutex_t finishMutex;
static pthread_cond_t mainWait;

void *runOut(void *data) {
    Task *task = (Task *) data;

    FILE *pipe = fdopen(task->outDsc, "r");
    if (!pipe) {
        fatal("Function fdopen failed.");
    }

    char myLine[MAX_OUT_LENGTH];
    while (read_line(myLine, MAX_OUT_LENGTH, pipe)) {
        ASSERT_ZERO(pthread_mutex_lock(&task->mutexOut));
        strcpy(task->out, myLine);
        ASSERT_ZERO(pthread_mutex_unlock(&task->mutexOut));
    }

    ASSERT_SYS_OK(fclose(pipe));

    int status;
    ASSERT_SYS_OK(waitpid(task->runnerPid, &status, 0));
    ASSERT_ZERO(pthread_join(task->errWriter, NULL));

    waitingToFinish++;
    ASSERT_ZERO(pthread_mutex_lock(&finishMutex));
    waitingToFinish--;

    if (WIFSIGNALED(status)) {
        printf("Task %zu ended: signalled.\n", task->id);
    } else {
        printf("Task %zu ended: status %d.\n", task->id, WEXITSTATUS(status));
    }

    if (waitingToFinish == 0) {
        ASSERT_ZERO(pthread_cond_signal(&mainWait));
    }

    ASSERT_ZERO(pthread_mutex_unlock(&finishMutex));

    return NULL;
}

void *runErr(void *data) {
    Task *task = (Task *) data;

    FILE *pipe = fdopen(task->errDsc, "r");
    if (!pipe) {
        fatal("Function fdopen failed.");
    }

    char myLine[MAX_OUT_LENGTH];
    while (read_line(myLine, MAX_OUT_LENGTH, pipe)) {
        ASSERT_ZERO(pthread_mutex_lock(&task->mutexErr));
        strcpy(task->err, myLine);
        ASSERT_ZERO(pthread_mutex_unlock(&task->mutexErr));
    }

    ASSERT_SYS_OK(fclose(pipe));

    return NULL;
}

void runTask(size_t id, char **args) {
    int out_dsc[2];
    int err_dsc[2];
    ASSERT_SYS_OK(pipe(out_dsc));
    ASSERT_SYS_OK(pipe(err_dsc));
    set_close_on_exec(out_dsc[0], true);
    set_close_on_exec(err_dsc[0], true);
    set_close_on_exec(out_dsc[1], true);
    set_close_on_exec(err_dsc[1], true);

    pid_t pid = fork();
    if (!pid) {
        ASSERT_SYS_OK(close(out_dsc[0]));
        ASSERT_SYS_OK(close(err_dsc[0]));

        ASSERT_SYS_OK(dup2(out_dsc[1], STDOUT_FILENO));
        ASSERT_SYS_OK(dup2(err_dsc[1], STDERR_FILENO));

        ASSERT_SYS_OK(close(out_dsc[1]));
        ASSERT_SYS_OK(close(err_dsc[1]));

        ASSERT_SYS_OK(execvp(args[0], args));
    } else {
        ASSERT_SYS_OK(close(out_dsc[1]));
        ASSERT_SYS_OK(close(err_dsc[1]));

        tasks[id].runnerPid = pid;
        tasks[id].outDsc = out_dsc[0];
        tasks[id].errDsc = err_dsc[0];

        ASSERT_ZERO(pthread_create(&tasks[id].errWriter, NULL, runErr,
                                   (void *) &tasks[id]));
        ASSERT_ZERO(pthread_create(&tasks[id].outWriter, NULL, runOut,
                                   (void *) &tasks[id]));

        printf("Task %zu started: pid %d.\n", id, pid);
    }
}

void initTask(size_t id) {
    ASSERT_ZERO(pthread_mutex_init(&(tasks[id].mutexOut), NULL));
    ASSERT_ZERO(pthread_mutex_init(&(tasks[id].mutexErr), NULL));
    memset(tasks[id].out, '\0', MAX_OUT_LENGTH);
    memset(tasks[id].err, '\0', MAX_OUT_LENGTH);
    tasks[id].id = id;
}

void destroyTask(size_t id) {
    ASSERT_ZERO(pthread_mutex_destroy(&tasks[id].mutexOut));
    ASSERT_ZERO(pthread_mutex_destroy(&tasks[id].mutexErr));
}

void out(size_t id) {
    ASSERT_ZERO(pthread_mutex_lock(&tasks[id].mutexOut));
    printf("Task %zu stdout: '%s'.\n", id, tasks[id].out);
    ASSERT_ZERO(pthread_mutex_unlock(&tasks[id].mutexOut));
}

void err(size_t id) {
    ASSERT_ZERO(pthread_mutex_lock(&tasks[id].mutexErr));
    printf("Task %zu stderr: '%s'.\n", id, tasks[id].err);
    ASSERT_ZERO(pthread_mutex_unlock(&tasks[id].mutexErr));
}

void killTask(size_t id) {
    kill(tasks[id].runnerPid, SIGINT);
}

void sleepMili(size_t timeToSleep) {
    usleep(timeToSleep * MILI_TO_MICRO);
}

size_t getId(char *line) {
    char **args = split_string(line);
    size_t id = strtoul(args[1], NULL, 0);
    free_split_string(args);

    return id;
}

void cleanUp(size_t runnedTasks) {
    for (size_t i = 0; i < runnedTasks; i++) {
        ASSERT_ZERO(pthread_join(tasks[i].outWriter, NULL));
        destroyTask(i);
    }

    ASSERT_ZERO(pthread_mutex_destroy(&finishMutex));
    ASSERT_ZERO(pthread_cond_destroy(&mainWait));
}

void quit(size_t runnedTasks) {
    for (size_t i = 0; i < runnedTasks; i++) {
        kill(tasks[i].runnerPid, SIGKILL);
    }
}

int main() {
    ASSERT_ZERO(pthread_mutex_init(&finishMutex, NULL));
    ASSERT_ZERO(pthread_cond_init(&mainWait, NULL));
    waitingToFinish = 0;
    char line[MAX_IN_LENGTH];
    char **args = NULL;
    size_t runId = 0;
    bool isEOF;

    while (true) {
        isEOF = !read_line(line, MAX_IN_LENGTH, stdin);
        ASSERT_ZERO(pthread_mutex_lock(&finishMutex));
        while (waitingToFinish > 0) {
            ASSERT_ZERO(pthread_cond_wait(&mainWait, &finishMutex));
        }

        if (strncmp(line, RUN, RUN_LENGTH) == 0) {
            args = split_string(line + RUN_LENGTH + SPACE_LENGTH);
            initTask(runId);
            runTask(runId, args);
            free_split_string(args);
            runId++;
        } else if (strncmp(line, SLEEP, SLEEP_LENGTH) == 0) {
            sleepMili(getId(line));
        } else if (strncmp(line, ERR, ERR_LENGTH) == 0) {
            err(getId(line));
        } else if (strncmp(line, OUT, OUT_LENGTH) == 0) {
            out(getId(line));
        } else if (strncmp(line, KILL, KILL_LENGTH) == 0) {
            killTask(getId(line));
        } else if (isEOF || strncmp(line, QUIT, QUIT_LENGTH) == 0) {
            quit(runId);
            ASSERT_ZERO(pthread_mutex_unlock(&finishMutex));
            cleanUp(runId);
            break;
        }

        ASSERT_ZERO(pthread_mutex_unlock(&finishMutex));
    }

    return 0;
}