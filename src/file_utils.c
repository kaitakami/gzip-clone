#include "file_utils.h"
#include "logging.h"
#include <unistd.h>
#include <sys/types.h>
#include <utime.h>

void get_file_attributes(const char *path, FileAttributes *attr) {
    struct stat st;
    if (stat(path, &st) != 0) {
        LOG_ERROR("Failed to get attributes for file: %s", path);
        return;
    }

    attr->mode = st.st_mode;
    attr->uid = st.st_uid;
    attr->gid = st.st_gid;
    attr->atime = st.st_atime;
    attr->mtime = st.st_mtime;
}

void set_file_attributes(const char *path, const FileAttributes *attr) {
    if (chown(path, attr->uid, attr->gid) != 0) {
        LOG_WARN("Failed to set ownership for file: %s", path);
    }

    if (chmod(path, attr->mode) != 0) {
        LOG_WARN("Failed to set permissions for file: %s", path);
    }

    struct utimbuf times;
    times.actime = attr->atime;
    times.modtime = attr->mtime;
    if (utime(path, &times) != 0) {
        LOG_WARN("Failed to set timestamps for file: %s", path);
    }
}
