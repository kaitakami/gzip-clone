#include "threadpool.h"
#include <stdlib.h>

static void *worker(void *arg) {
    threadpool_t *pool = (threadpool_t *)arg;
    task_t task;

    while (1) {
        pthread_mutex_lock(&(pool->lock));

        while ((pool->count == 0) && (!pool->stop)) {
            pthread_cond_wait(&(pool->not_empty), &(pool->lock));
        }

        if (pool->stop) {
            pthread_mutex_unlock(&(pool->lock));
            pthread_exit(NULL);
        }

        task.function = pool->queue[pool->head].function;
        task.argument = pool->queue[pool->head].argument;

        pool->head = (pool->head + 1) % pool->queue_size;
        pool->count--;

        pthread_cond_signal(&(pool->not_full));
        pthread_mutex_unlock(&(pool->lock));

        (*(task.function))(task.argument);
    }

    return NULL;
}

threadpool_t *threadpool_create(int num_threads, int queue_size) {
    threadpool_t *pool;
    int i;

    pool = (threadpool_t *)malloc(sizeof(threadpool_t));
    pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * num_threads);
    pool->queue = (task_t *)malloc(sizeof(task_t) * queue_size);

    pool->queue_size = queue_size;
    pool->num_threads = num_threads;
    pool->head = pool->tail = pool->count = 0;
    pool->stop = 0;

    pthread_mutex_init(&(pool->lock), NULL);
    pthread_cond_init(&(pool->not_empty), NULL);
    pthread_cond_init(&(pool->not_full), NULL);

    for (i = 0; i < num_threads; i++) {
        pthread_create(&(pool->threads[i]), NULL, worker, pool);
    }

    return pool;
}

void threadpool_destroy(threadpool_t *pool) {
    int i;

    pthread_mutex_lock(&(pool->lock));
    pool->stop = 1;
    pthread_cond_broadcast(&(pool->not_empty));
    pthread_mutex_unlock(&(pool->lock));

    for (i = 0; i < pool->num_threads; i++) {
        pthread_join(pool->threads[i], NULL);
    }

    pthread_mutex_destroy(&(pool->lock));
    pthread_cond_destroy(&(pool->not_empty));
    pthread_cond_destroy(&(pool->not_full));

    free(pool->threads);
    free(pool->queue);
    free(pool);
}

int threadpool_add_task(threadpool_t *pool, void (*function)(void *), void *argument) {
    pthread_mutex_lock(&(pool->lock));

    while ((pool->count == pool->queue_size) && (!pool->stop)) {
        pthread_cond_wait(&(pool->not_full), &(pool->lock));
    }

    if (pool->stop) {
        pthread_mutex_unlock(&(pool->lock));
        return -1;
    }

    pool->queue[pool->tail].function = function;
    pool->queue[pool->tail].argument = argument;
    pool->tail = (pool->tail + 1) % pool->queue_size;
    pool->count++;

    pthread_cond_signal(&(pool->not_empty));
    pthread_mutex_unlock(&(pool->lock));

    return 0;
}
