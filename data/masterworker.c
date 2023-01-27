#define _GNU_SOURCE
#include <queue.h>
#include <utils.h>
#include <masterworker.h>
#include <workers.h>
// #include <unistd.h>


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtx_time = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cond_notEmpty = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_time = PTHREAD_COND_INITIALIZER;

extern volatile sig_atomic_t waitexit;
extern volatile sig_atomic_t printcurrentresult;

node *shared_queue;


volatile int endworker = 0;

extern int mastercollector[2];

void masterfun(int numworkers, node **support_queue, int dimqueue, int timetowait)
{
    int err;
    int status;
    int fd_client;

    // creo il pool di threads
    create_workers(numworkers);

    fd_set set;
    FD_ZERO(&set);
    FD_SET(fds, &set);

    SYSCALL(fd_client,accept(fds, NULL, NULL),"accept masterworker");


    size_t lensupplist = lenght(*support_queue);

    int i = 0;

    while (!interrupt || i < lensupplist)
    {
        //se il tempo tra una richiesta e l'altra è stata impostato
        if(timetowait != 0)  Pthread_mutex_lock(&mtx_time);
        
        node *curr = pop(support_queue);
        if (curr == NULL)
        {
            break;
        }

        
        Pthread_mutex_lock(&mutex);
        //lista condivisa ha raggiunto il limite impostato
        while (lenght(shared_queue) == dimqueue)
        {
            printf("lista piena\n");
            Pthread_cond_wait(&cond_full, &mutex);
        }
        //sostituire shared_queue con addtail
        addTail(&shared_queue, curr->data, fd_client);
        free(curr->data);
        free(curr);

        Pthread_cond_signal(&cond_notEmpty);
        Pthread_mutex_unlock(&mutex);
        if (timetowait != 0)
        {
            
            struct timeval timeday;
            struct timespec timerequest;
            gettimeofday(&timeday,NULL);
            timerequest.tv_sec = timeday.tv_sec;
            timerequest.tv_nsec = timeday.tv_usec * 1000;
            timerequest.tv_sec += timetowait/1000;

            int check = pthread_cond_timedwait(&cond_time, &mtx_time, &timerequest);
            // se scade il timer
            if (check == ETIMEDOUT)
            {
                Pthread_mutex_unlock(&mtx_time);
            }
            else break;
        }
        i++;
    }
    // printf("\t[MASTER]: esco\n");
    Pthread_mutex_unlock(&mtx_time);
    endworker = 1;
    Pthread_cond_broadcast(&cond_notEmpty);
    for (int i = 0; i < numworkers; i++)
    {
        if ((pthread_join(workersPool[i], NULL)) != 0)
        {
            perror("pthread join");
        }
    }



    close(fd_client);


    SYSCALL(err,waitpid(-1, &status, 0),"waitpid");

    free(worker);
    free(workersPool);
    Pthread_mutex_lock(&mutex);
    freeQueue(&shared_queue);
    freeQueue(support_queue);
    Pthread_mutex_unlock(&mutex);
}

void requestprint()
{
    int err;
    int print = 1;
    SYSCALL(err, write(mastercollector[1], &print, sizeof(int)), "write pipe");
    print = 0;
}