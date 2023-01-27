#define _GNU_SOURCE
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
#include <sys/mman.h>
#include <fcntl.h>

#include <queue.h>
#include <utils.h>
#include <file.h>
#include <collector.h>
#include <masterworker.h>

#define SOCKNAME "./farm.sck"
#define UNIX_MAX_PATH 108
#define MAXPATHNAME 255

int fds = 0; // file descriptor del socket

volatile sig_atomic_t waitexit = 0;
volatile sig_atomic_t printcurrentresult = 0;

int mastercollector[2];

int interrupt = 0;

extern pthread_mutex_t mutex;
extern pthread_mutex_t mtx_time;

extern pthread_cond_t cond_notEmpty;
extern pthread_cond_t cond_full;
extern pthread_cond_t cond_time;

node *support_queue = NULL;

void cleanup()
{
    unlink(SOCKNAME);
}

void *signalroutine(void *arg)
{

    sigset_t *set = arg;
    int s, sig;

    while (1)
    {
        s = sigwait(set, &sig);
        if (s != 0)
        {
            errno = s;
            perror("sigwait");
            exit(EXIT_FAILURE);
        }
        if (sig == SIGINT)
        {
            printf("SIGINT ricevuto\n");
            Pthread_cond_signal(&cond_time);
            interrupt = 1;
            waitexit = 1;

            // exit(0);
        }
        if (sig == SIGQUIT)
        {
            printf("SIGQUIT ricevuto\n");
            Pthread_cond_signal(&cond_time);
            interrupt = 1;
            waitexit = 1;

            // exit(0);
        }
        if (sig == SIGTERM)
        {
            // printf("SIGTERM ricevuto\n");
            Pthread_cond_signal(&cond_time);
            interrupt = 1;
            waitexit = 1;
            // exit(0);
        }

        if (sig == SIGUSR1)
        {
            printf("SIGUSR1 ricevuto\n");
            requestprint();
        }
        //segnale per terminare il thread
        if (sig == SIGUSR2)
        {
            pthread_exit(0);
        }

        if (sig == SIGHUP)
        {
            printf("SIGHUP ricevuto\n");
            Pthread_cond_signal(&cond_time);
            interrupt = 1;
            waitexit = 1;
        }
    }
    pthread_exit(0);
}

int main(int argc, char *argv[])
{

    int err;

    cleanup();
    if ((atexit(cleanup) != 0))
        exit(EXIT_FAILURE);

    pthread_t sighandler_thread;
    sigset_t sigset;

    sigemptyset(&sigset);

    sigaddset(&sigset, SIGUSR1);
    sigaddset(&sigset, SIGUSR2);
    sigaddset(&sigset, SIGINT);
    sigaddset(&sigset, SIGTERM);
    sigaddset(&sigset, SIGQUIT);
    sigaddset(&sigset, SIGHUP);

    err = pthread_sigmask(SIG_BLOCK, &sigset, NULL);
    if (err != 0)
    {
        perror("pthread_sigmask");
        exit(EXIT_FAILURE);
    }

    struct sigaction siga;
    memset(&siga, 0, sizeof(siga));
    // SIGPIPE viene ignorato
    siga.sa_handler = SIG_IGN;
    SYSCALL(err, sigaction(SIGPIPE, &siga, NULL), "sigaction SIGPIPE");

    err = pthread_create(&sighandler_thread, NULL, signalroutine, (void *)&sigset);
    if (err != 0)
    {
        errno = err;
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    pid_t pid;
    struct sockaddr_un sa;
    sa.sun_family = AF_UNIX;

    strncpy(sa.sun_path, SOCKNAME, UNIX_MAX_PATH);

    SYSCALL(err, pipe(mastercollector), "creazione pipe");

    SYSCALL(fds, socket(AF_UNIX, SOCK_STREAM, 0), "socket");

    SYSCALL(err, bind(fds, (struct sockaddr *)&sa, sizeof(sa)), "bind");

    SYSCALL(err, listen(fds, SOMAXCONN), "listen");

    pid = fork();
    if (pid == -1)
    {
        printf("fork non riuscita\n");
        exit(EXIT_FAILURE);
    }
    if (pid != 0)
    {

        close(mastercollector[0]);
        
        int nthreads = 4;
        int dimqueue = 8;

        int timetowait = 0;

        struct stat filestat;
        int cmd = 0, dflag = 0;
        // variabili usate per controllare se gli argomenti dei comandi sono corretti
        int wrongNargument = 0, wrongQargument = 0, wrongTargument = 0;
        char *directory = NULL;
        long arg;
        opterr = 0;

        while ((cmd = getopt(argc, argv, "n:q:t:d:")) != -1)
        {
            switch (cmd)
            {
            case 'n':
            {

                arg = isNumber(optarg);
                if (arg == -1 || arg<=0)
                {
                    fprintf(stderr, "%s", "l'argomento passato come numero di worker non è valido\n");
                    errno = EINVAL;

                    if ((pthread_kill(sighandler_thread, 12)) != 0)
                    {
                        perror("pthread kill");
                    }
                    wrongNargument = 1;
                }
                else
                {
                    nthreads = arg;
                }

                break;
            }

            case 'q':
            {
                arg = isNumber(optarg);
                if (arg == -1 || arg<=0)
                {
                    fprintf(stderr, "%s", "l'argomento passato come dimensione della lista non è valido\n");
                    errno = EINVAL;
                    if ((pthread_kill(sighandler_thread, 12)) != 0)
                    {
                        perror("pthread kill");
                        exit(EXIT_FAILURE);
                    }
                    wrongQargument = 1;
                }
                dimqueue = arg;

                break;
            }

            case 'd':
            {

                size_t lendir = strlen(optarg) + 1;
                directory = malloc(sizeof(char) * lendir);
                NULL_CHECK(directory);
                strncpy(directory, optarg, lendir);
                dflag = 1;

                break;
            }

            case 't':
            {
                if ((arg = isNumber(optarg)) == -1)
                {
                    fprintf(stderr, "%s", "l'argomento passato come tempo tra le richieste non è valido\n");
                    errno = EINVAL;
                    if ((pthread_kill(sighandler_thread, 12)) != 0)
                    {
                        perror("pthread kill");
                        exit(EXIT_FAILURE);
                    }
                    wrongTargument = 1;
                }
                timetowait = arg;
                break;
            }
            }
        }

        int index;
        size_t lenname;
        char fullname[MAXPATHNAME];

        if (!wrongQargument && !wrongNargument && !wrongTargument)
        {
            for (index = optind; index < argc; index++)
            {

                // controllo sulla lunghezza del nome del file
                lenname = strlen(argv[index] + 1);
                if (lenname > MAXPATHNAME)
                {
                    errno = ENAMETOOLONG;
                    perror("nome file");
                }

                else
                {
                    memset(fullname, 0, MAXPATHNAME);
                    memcpy(fullname, argv[index], (strlen(argv[index]) + 1));
                    is_correct_file(fullname, filestat, &support_queue);
                }
            }

            //se la cartella è stata specificata viene visitata ricorsivamente
            if (dflag)
            {
                recursiveSearch(directory, &support_queue);
            }
        }
        masterfun(nthreads, &support_queue, dimqueue, timetowait);

        close(mastercollector[1]);

        // viene fatta la free se il nome della cartella è stato specificato
        if (directory != NULL)
            free(directory);

        if ((pthread_kill(sighandler_thread, 12)) != 0)
        {
            perror("pthread kill");
            exit(EXIT_FAILURE);
        }

        if ((pthread_join(sighandler_thread, NULL)) != 0)
        {
            perror("pthread join");
            exit(EXIT_FAILURE);
        }
    }
    else // PROCESSO COLLECTOR
    {
        
        close(mastercollector[1]);
        executeCollector(SOCKNAME);
    }

    return 0;
}
