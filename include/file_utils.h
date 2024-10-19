#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <sys/stat.h>
#include <time.h>

typedef struct {
    mode_t mode;
    uid_t uid;
    gid_t gid;
    time_t atime;
    time_t mtime;
} FileAttributes;

void get_file_attributes(const char *path, FileAttributes *attr);
void set_file_attributes(const char *path, const FileAttributes *attr);

#endif // FILE_UTILS_H
