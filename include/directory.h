#ifndef DIRECTORY_H
#define DIRECTORY_H

#include "config.h"

#define MAX_PATH 1024

typedef struct {
    char file_path[MAX_PATH];
    Config *config;
} file_task_t;

int process_file(const char *file_path, Config *config);
int process_directory(const char *dir_path, Config *config);
void process_file_task(void *arg);

#endif // DIRECTORY_H
