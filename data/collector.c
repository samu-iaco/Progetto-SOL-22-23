

#include <collector.h>

extern volatile sig_atomic_t waitexit;

extern int mastercollector[2];

int endconnection = 0;
collectedFiles *received_file = NULL;




void blockSignal(sigset_t *set)
{

    int err;
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sigemptyset(set);

    sa.sa_handler = SIG_IGN;
    SYSCALL(err, sigaction(SIGPIPE, &sa, NULL), "Sigaction SIGPIPE");
    sigaddset(set,SIGINT);
    sigaddset(set,SIGTERM);
    sigaddset(set, SIGHUP);
    sigaddset(set, SIGUSR1);
    sigaddset(set, SIGQUIT);
    sigprocmask(SIG_BLOCK, set, NULL);
}

void executeCollector(const char *sockname)
{

    printf("avvio collector\n");
    // maschera segnali
    sigset_t collectorset;
    blockSignal(&collectorset);

    
    int sfd;
    int len;
    int err;
    int reading;
    int readingpipe;
    int print = 0;
    long result;
    char *file;

    // Creo il socket
    SYSCALL(sfd, socket(AF_UNIX, SOCK_STREAM, 0), "Client - Error creating socket");

    // Setto l'indirizzo
    struct sockaddr_un serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strncpy(serv_addr.sun_path, sockname, strlen(sockname) + 1);

    // Connetto il socket e l'indirizzo
    int unused;
    SYSCALL(unused, connect(sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)), "Client - Error connecting socket");


    fd_set set, rdset;
    int fd_num = 0, fd;
    FD_ZERO(&set);
    FD_SET(sfd, &set);
    FD_SET(mastercollector[0], &set);
    if (sfd > fd_num)
        fd_num = sfd;

    while (!endconnection)
    {
        rdset = set;
        if (select(fd_num + 1, &rdset, NULL, NULL, NULL) == -1)
        {
            perror("select");
            break;
        }

        for (fd = 0; fd <= fd_num; fd++)
        {
            if (FD_ISSET(fd, &rdset))
            {

                if (fd == mastercollector[0])
                {


                    SYSCALL(readingpipe,read(mastercollector[0], &print, sizeof(int)),"reading from pipe");
                    if(readingpipe == 0) break;
                    
                    //richiesta di stampa della lista da parte del master
                    if(print == 1){
                        mergesort(&received_file);
                        printQueuecollector(received_file);
                        print = 0;
                    }


                    
                }
        
                
                else
                {
                    
                    reading = read(sfd, &len, sizeof(int));


                    if (reading == -1)
                    {
                        printf("\t\t[COLLECTOR]: errore lettura\n");
                        endconnection = 1;
                        break;
                    }

                    if (reading == 0)
                    {
                        printf("\t\t[COLLECTOR]: finito lettura\n");
                        FD_CLR(sfd,&set);
                        endconnection = 1;
                        break;
                    }


                    file = malloc(sizeof(char) * (len + 1));
                    NULL_CHECK(file);

                    SYSCALL(err, read(sfd, file, len), "read file");

                    file[len] = '\0';


                    SYSCALL(err, read(sfd, &result, sizeof(long)), "read result");

                    //aggiungo il file alla lista del collector
                    addHeadcollector(&received_file, file, result);
                    free(file);

                    
                }
            }
        }
        

        
    }
    //ordino la lista con mergesort
    mergesort(&received_file);
    
    //stampo la lista
    printQueuecollector(received_file);

    //free della lista
    freeQueuecollector(&received_file);

    close(mastercollector[0]);
  
    exit(0);

}