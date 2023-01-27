#ifndef queue
#define queue

typedef struct _node
{
    char *data;
    int fd;
    struct _node *next;
}node;


typedef struct _collectedFiles
{
    char *data;
    long sum;
    struct _collectedFiles *next;
}collectedFiles;


int lenght(node *q);

void addHead(node **head, char *val, int fd);
void addTail(node **head, char *val, int fd);

void addHeadcollector(collectedFiles **head, char *val, long result);

void printQueue(node *q);
void printQueuecollector(collectedFiles *q);

void freeQueue(node **head);
void freeQueuecollector(collectedFiles **head);

void mergesort(collectedFiles **headRef);

node *getlast(node *q);

node* pop(node **head);



#endif