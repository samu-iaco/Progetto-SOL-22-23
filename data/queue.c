#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <queue.h>

void addHead(node **head, char *val, int fd)
{
    node *new = malloc(sizeof(node));
    if (new == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    int len = strlen(val) + 1;
    new->data = malloc(sizeof(char) * len);
    if (new->data == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    memset(new->data, 0, len);
    memcpy(new->data, val, len);
    new->fd = fd;
    new->next = *head;

    *head = new;
}

void addHeadcollector(collectedFiles **head, char *val, long result)
{

    collectedFiles *new = malloc(sizeof(collectedFiles));
    if (new == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    int len = strlen(val) + 1;
    new->data = malloc(sizeof(char) * len);
    if (new->data == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    memset(new->data, 0, len);
    memcpy(new->data, val, len);
    new->sum = result;
    new->next = *head;

    *head = new;
}

node *pop(node **head)
{
    if (*head == NULL)
    {
        return NULL;
    }

    node *tmp = *head;


    (*head) = (*head)->next;

    return tmp;
}

void addTail(node **head, char *val, int fd)
{

    node *tmp = malloc(sizeof(node));
    if (tmp == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    node *currHead = *head;
    tmp->fd = fd;

    int len = strlen(val) + 1;
    tmp->data = malloc(sizeof(char) * len);
    if (tmp->data == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    memset(tmp->data, 0, len);
    memcpy(tmp->data, val, len);
    tmp->next = NULL;
    // currHead = *head;
    if (currHead == NULL)
    {
        *head = tmp;
    }
    else
    {
        while (currHead->next != NULL)
        {
            currHead = currHead->next;
        }
        currHead->next = tmp;
    }
    // printQueue(*head);
}

int lenght(node *q)
{
    node *curr = q;
    int count = 0;
    while (curr != NULL)
    {
        count++;
        curr = curr->next;
    }

    return count;
}

void printQueue(node *q)
{
    node *curr = q;
    while (curr != NULL)
    {
        printf("%s %d\n", curr->data, curr->fd);
        curr = curr->next;
    }
}

void printQueuecollector(collectedFiles *q)
{
    collectedFiles *curr = q;
    while (curr != NULL)
    {
        printf("%ld %s\n", curr->sum, curr->data);
        curr = curr->next;
    }
}

void splitlist(collectedFiles *source, collectedFiles **front, collectedFiles **back)
{
    collectedFiles *fast;
    collectedFiles *slow;
    slow = source;
    fast = source->next;

    //fast avanza di 2 posizioni mentre slow di 1
    while (fast != NULL)
    {
        fast = fast->next;
        if (fast != NULL)
        {
            slow = slow->next;
            fast = fast->next;
        }
    }

    //front sarà la prima metà mentre back la seconda metà
    *front = source;
    *back = slow->next;
    slow->next = NULL;

}

collectedFiles *merge(collectedFiles *a, collectedFiles *b)
{
    collectedFiles *result = NULL;




    //casi base
    if (a == NULL)
        return b;
    else if (b == NULL)
        return a;

    //la lista viene creata ordinata scegliendo il minore tra a e b
    if (a->sum <= b->sum)
    {
        result = a;
        // printf("\t\taggiungo %s alla lista\n", a->data);
        result->next = merge(a->next, b);
    }
    else
    {
        result = b;
        // printf("\t\taggiungo %s alla lista\n", b->data);
        result->next = merge(a, b->next);
    }


    return (result);
}

// ordino la lista con il mergesort
void mergesort(collectedFiles **list)
{
    collectedFiles *head = *list;
    collectedFiles *a;
    collectedFiles *b;

    //la lista è vuota o ha un solo elemento
    if ((head == NULL) || (head->next == NULL))
    {
        return;
    }

    //divido la lista
    splitlist(head, &a, &b);


    //le 2 liste vengono ordinate
    mergesort(&a);
    mergesort(&b);




    //viene fatto il merge delle 2 liste
    *list = merge(a, b);
}

void freeQueue(node **head)
{
    while (*head != NULL)
    {
        node *delete = *head;
        *head = delete->next;
        free(delete->data);
        free(delete);
    }
}

void freeQueuecollector(collectedFiles **head)
{
    while (*head != NULL)
    {
        collectedFiles *delete = *head;
        *head = delete->next;

        free(delete->data);
        free(delete);
    }
}