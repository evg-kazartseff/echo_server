//
// Created by evgenii on 29.01.18.
//

#include "Queue.h"

int QueueCreate(queue_t **Queue) {
    queue_t* queue = malloc(sizeof(queue_t));
    if (!queue) {
        return 1;
    }
    queue->size = 0;
    pthread_mutex_init(&queue->queue_mutex, NULL);
    *Queue = queue;
    return 0;
}

int QueueInsert(queue_t *Queue, int value) {
    if (!Queue) {
        return 1;
    }
    queue_node_t* new_node = malloc(sizeof(queue_node_t));
    if (!new_node) {
        return 1;
    }
    new_node->sd = value;
    pthread_mutex_lock(&Queue->queue_mutex);
    queue_node_t* tmp = Queue->head;
    if (tmp) {
        while (tmp->next)
            tmp = tmp->next;
        tmp->next = new_node;
    } else {
        Queue->head = new_node;
    }
    Queue->size++;
    pthread_mutex_unlock(&Queue->queue_mutex);
    return 0;
}

int QueueErase(queue_t *Queue, int *value) {
    if (!Queue) {
        return 1;
    }
    pthread_mutex_lock(&Queue->queue_mutex);
    queue_node_t* tmp = Queue->head;
    if (tmp) {
        *value = tmp->sd;
        Queue->head = tmp->next;
        free(tmp);
        Queue->size--;
        pthread_mutex_unlock(&Queue->queue_mutex);
    }
    else {
        pthread_mutex_unlock(&Queue->queue_mutex);
        return 1;
    }
    return 0;
}

int QueueGetSize(queue_t *Queue, unsigned long *size) {
    if (!Queue) {
        return 1;
    }
    pthread_mutex_lock(&Queue->queue_mutex);
    *size = Queue->size;
    pthread_mutex_unlock(&Queue->queue_mutex);
    return 0;
}



