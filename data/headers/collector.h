#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <ctype.h>
#include <sys/un.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/stat.h>

#include <utils.h>
#include <queue.h>


extern int mastercollector[2];



void executeCollector(const char *sockname);
