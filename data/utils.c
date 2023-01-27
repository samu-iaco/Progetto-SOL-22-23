#include <limits.h>

#include <utils.h>

void Pthread_mutex_lock(pthread_mutex_t *mtx)
{
    int err;
    if ((err = pthread_mutex_lock(mtx)) != 0)
    {
        errno = err;
        perror("pthread_lock");
        exit(EXIT_FAILURE);
    }
}

void Pthread_mutex_unlock(pthread_mutex_t *mtx)
{
    int err;
    if ((err = pthread_mutex_unlock(mtx)) != 0)
    {
        errno = err;
        perror("pthread_unlock");
        exit(EXIT_FAILURE);
    }
}

void Pthread_cond_signal(pthread_cond_t *cond)
{
    int err;
    if ((err = pthread_cond_signal(cond)) != 0)
    {
        errno = err;
        perror("pthread_signal");
        exit(EXIT_FAILURE);
    }
}

void Pthread_cond_wait(pthread_cond_t *cond_notEmpty, pthread_mutex_t *mtx){
    int err;
    if ((err = pthread_cond_wait(cond_notEmpty,mtx)) != 0)
    {
        errno = err;
        perror("pthread_wait");
        exit(EXIT_FAILURE);
    }

}

void Pthread_cond_broadcast(pthread_cond_t *cond){
    int err;
    if ((err = pthread_cond_broadcast(cond)) != 0)
    {
        errno = err;
        perror("pthread_broadcast");
        exit(EXIT_FAILURE);
    }

}

long isNumber(const char* s) {
   char* e = NULL;
   long val = strtol(s, &e, 0);
   if (e != NULL && *e == (char)0) return val; 
   return -1;
}
