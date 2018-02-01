//
// Created by evgenii on 29.01.18.
//

#ifndef ECHO_QUEUE_H
#define ECHO_QUEUE_H

#include <stdlib.h>
#include <pthread.h>

struct queue_node {
    int sd;
    struct queue_node* next;
};

struct queue {
    struct queue_node* head;
    unsigned long size;
    pthread_mutex_t queue_mutex;
};

typedef struct queue_node queue_node_t;
typedef struct queue queue_t;

int QueueCreate(queue_t** Queue);
int QueueInsert(queue_t* Queue, int value);
int QueueErase(queue_t* Queue, int* value);
int QueueGetSize(queue_t* Queue, unsigned long* size);

#endif //ECHO_QUEUE_H
