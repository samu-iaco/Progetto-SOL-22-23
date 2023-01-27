#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>

#include <queue.h>

extern int fds;
extern int interrupt;

extern int mastercollector[2];

void masterfun(int numworkers, node **support_queue, int dimqueue, int timetowait);

void requestprint();


void close_workers(int numworkers);