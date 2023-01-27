#include <workers.h>

extern volatile sig_atomic_t waitexit;
extern volatile sig_atomic_t printcurrentresult;

extern pthread_mutex_t mutex;
extern pthread_cond_t cond_notEmpty;
extern pthread_cond_t cond_full;

pthread_mutex_t mtx_socket = PTHREAD_MUTEX_INITIALIZER;

extern volatile int endworker;

thworker *worker;
pthread_t *workersPool;

void executeTask(node *nodo, int tid)
{
    int fdcollector = nodo->fd;
    int err;
    int lennamefile;
    int lines = 0;
    long result = 0;
    fflush(stdout);


    FILE *fp = NULL;
    fp = fopen(nodo->data, "rb");
    if (fp == NULL)
    {
        perror("apertura file binario\n");
        free(nodo->data);
        free(nodo);
        return;
    }

    if (fseek(fp, 0, SEEK_END) != 0)
    {
        perror("fseek");
        if( (fclose(fp)) != 0 ){
            perror("fclose");
            exit(EXIT_FAILURE);
        }
        return;
    }

    int lengthOfFile = ftell(fp);

    if (lengthOfFile == -1)
    {
        perror("ftell");
        fclose(fp);
        return;
    }

    rewind(fp);

    lines = lengthOfFile / sizeof(long);

    long bufferlong[lines];
    int fileread = 0;
    fileread = fread(bufferlong, sizeof(long), lines, fp);

    rewind(fp);

    for (int i = 0; i < fileread; i++)
    {
        result += i * bufferlong[i];
    }

    if( (fclose(fp)) != 0 ){
        perror("fclose");
        exit(EXIT_FAILURE);
    }
    lennamefile = strlen(nodo->data) + 1;
    Pthread_mutex_lock(&mtx_socket);
    SYSCALL(err, write(fdcollector, &lennamefile, sizeof(int)), "write lenght file");

    SYSCALL(err, write(fdcollector, nodo->data, lennamefile), "write file");

    SYSCALL(err, write(fdcollector, &result, sizeof(long)), "write result");
    Pthread_mutex_unlock(&mtx_socket);

    free(nodo->data);
    free(nodo);
}

void *startWorker(void *arg)
{
    fflush(stdout);
    int tid = ((thworker *)arg)->tid;


    while (!waitexit)
    {

        Pthread_mutex_lock(&mutex);
        while (lenght(shared_queue) == 0 && !endworker)
        {

            Pthread_cond_wait(&cond_notEmpty, &mutex);
        }

    

            node *nodo = pop(&shared_queue);
            if (nodo == NULL)
            {
                Pthread_mutex_unlock(&mutex);
                break;
            }
            Pthread_cond_signal(&cond_full);

            Pthread_mutex_unlock(&mutex);
            executeTask(nodo, tid);

    }

    pthread_exit(0);
}

void create_workers(int numworkers)
{
    int err;
    workersPool = malloc(sizeof(pthread_t) * numworkers);
    NULL_CHECK(workersPool);
    memset(workersPool, 0, (numworkers * sizeof(pthread_t)));

    worker = malloc(sizeof(thworker) * numworkers);
    NULL_CHECK(worker);

    for (int i = 0; i < numworkers; i++)
    {

        worker[i].tid = i;

        SYSCALL(err, pthread_create(&workersPool[i], NULL, startWorker, (void *)&worker[i]), "pthread_create");
    }
}