/**
 * @file utils.h
 * @brief header contenente macro per utilità
 * 
 */

#ifndef utils
#define utils

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>


#define SYSCALL(r,c,e) if((r=c)==-1) { \
    perror(e); \
    exit(errno); \
    }     

#define NULL_CHECK(val)  if (val == NULL) exit(EXIT_FAILURE);



void Pthread_mutex_lock(pthread_mutex_t *mtx);

void Pthread_mutex_unlock(pthread_mutex_t *mtx);

void Pthread_cond_signal(pthread_cond_t *cond);

void Pthread_cond_wait(pthread_cond_t *cond_notEmpty, pthread_mutex_t *mtx);

void Pthread_cond_broadcast(pthread_cond_t *cond);

long isNumber(const char* s);

#endif