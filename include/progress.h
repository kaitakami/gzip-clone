#ifndef PROGRESS_H
#define PROGRESS_H

#include <stdint.h>

typedef struct {
    uint64_t total_bytes;
    uint64_t processed_bytes;
    int last_percentage;
} ProgressInfo;

void init_progress(ProgressInfo *progress, uint64_t total_bytes);
void update_progress(ProgressInfo *progress, uint64_t processed_bytes);
void finish_progress(ProgressInfo *progress);

#endif // PROGRESS_H
