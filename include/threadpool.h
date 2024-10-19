#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include "config.h"

typedef struct {
    void (*function)(void *);
    void *argument;
} task_t;

typedef struct {
    task_t *queue;
    int queue_size;
    int head;
    int tail;
    int count;
    pthread_t *threads;
    int num_threads;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
    int stop;
} threadpool_t;

threadpool_t *threadpool_create(int num_threads, int queue_size);
void threadpool_destroy(threadpool_t *pool);
int threadpool_add_task(threadpool_t *pool, void (*function)(void *), void *argument);

#endif // THREADPOOL_H
