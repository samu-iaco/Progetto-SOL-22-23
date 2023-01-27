#define _POSIX_SOURCE 1
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include <queue.h>
#include <utils.h>

typedef struct _thworker
{
    int tid;
} thworker;


extern thworker *worker;
extern pthread_t *workersPool;

extern node *shared_queue;


extern volatile int endworker;



void submitFile(node *nodo, int tid);

void *startWorker(void *arg);

void create_workers(int numworkers);