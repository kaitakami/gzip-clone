#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "cli.h"
#include "config.h"
#include "compress.h"
#include "decompress.h"
#include "directory.h"
#include "threadpool.h"
#include "logging.h"

#define MAX_PATH 1024

int main(int argc, char *argv[]) {
    init_logger("gzip-clone.log", LOG_DEBUG);
    LOG_INFO("GZIP-Clone Initialized");
    
    Config config;
    init_config(&config);
    
    parse_arguments(argc, argv, &config);
    
    threadpool_t *pool = threadpool_create(config.threads, config.threads * 2);
    
    for (int i = 0; i < config.file_count; i++) {
        char *input_path = config.files[i];
        struct stat path_stat;
        
        if (stat(input_path, &path_stat) != 0) {
            LOG_ERROR("Cannot access '%s'", input_path);
            continue;
        }
        
        if (S_ISDIR(path_stat.st_mode)) {
            if (config.recursive) {
                LOG_INFO("Processing directory: %s", input_path);
                if (process_directory(input_path, &config) != 0) {
                    LOG_ERROR("Error processing directory: %s", input_path);
                }
            } else {
                LOG_WARN("'%s' is a directory. Use -r for recursive operation.", input_path);
            }
        } else if (S_ISREG(path_stat.st_mode)) {
            file_task_t *task = malloc(sizeof(file_task_t));
            strncpy(task->file_path, input_path, MAX_PATH);
            task->config = &config;
            threadpool_add_task(pool, process_file_task, task);
        } else {
            LOG_WARN("'%s' is not a regular file or directory", input_path);
        }
    }
    
    // Wait for all tasks to complete
    while (pool->count > 0) {
        usleep(1000); // Sleep for 1ms
    }
    
    threadpool_destroy(pool);
    LOG_INFO("GZIP-Clone finished processing");
    close_logger();
    return 0;
}
