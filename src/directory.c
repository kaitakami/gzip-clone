#include "directory.h"
#include "compress.h"
#include "decompress.h"
#include "threadpool.h"
#include "logging.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <libgen.h>

#define MAX_PATH 1024

void process_file_task(void *arg) {
    file_task_t *task = (file_task_t *)arg;
    LOG_DEBUG("Thread %p processing file: %s", (void*)pthread_self(), task->file_path);
    int result = process_file(task->file_path, task->config);
    if (result == 0) {
        LOG_INFO("Successfully processed: %s", task->file_path);
    } else {
        LOG_ERROR("Failed to process: %s", task->file_path);
    }
    free(task);
}

int process_file(const char *file_path, Config *config) {
    char output_file[MAX_PATH];
    char *base_name = basename((char *)file_path);
    char *dir_name = dirname((char *)file_path);
    
    if (config->compress) {
        snprintf(output_file, sizeof(output_file), "%s/%s.gz", dir_name, base_name);
        LOG_INFO("Compressing %s to %s", file_path, output_file);
        int result = compress_file_stream(file_path, output_file, config);
        if (result == 0) {
            printf("Compressed %s to %s\n", file_path, output_file);
        }
        return result;
    } else if (config->decompress) {
        size_t len = strlen(base_name);
        if (len > 3 && strcmp(base_name + len - 3, ".gz") == 0) {
            strncpy(output_file, file_path, MAX_PATH);
            output_file[strlen(output_file) - 3] = '\0';
        } else {
            snprintf(output_file, sizeof(output_file), "%s/%s.decompressed", dir_name, base_name);
        }
        LOG_INFO("Decompressing %s to %s", file_path, output_file);
        int result = decompress_file_stream(file_path, output_file, config);
        if (result == 0) {
            printf("Decompressed %s to %s\n", file_path, output_file);
        }
        return result;
    }
    
    return -1;
}

int process_directory(const char *dir_path, Config *config) {
    DIR *dir;
    struct dirent *entry;
    char path[MAX_PATH];
    struct stat statbuf;
    int status = 0;
    int file_count = 0;
    threadpool_t *pool = threadpool_create(config->threads, config->threads * 2);

    if ((dir = opendir(dir_path)) == NULL) {
        perror("opendir");
        return -1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);

        if (lstat(path, &statbuf) == -1) {
            perror("lstat");
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            if (config->recursive) {
                printf("Entering directory: %s\n", path);
                if (process_directory(path, config) != 0) {
                    status = -1;
                }
            }
        } else if (S_ISREG(statbuf.st_mode)) {
            file_count++;
            file_task_t *task = malloc(sizeof(file_task_t));
            strncpy(task->file_path, path, MAX_PATH);
            task->config = config;
            threadpool_add_task(pool, process_file_task, task);
        }
    }

    closedir(dir);
    
    // Wait for all tasks to complete
    while(pool->count > 0) {
        usleep(1000); // Sleep for 1ms
    }
    
    threadpool_destroy(pool);
    printf("Processed %d files in directory: %s\n", file_count, dir_path);
    return status;
}
