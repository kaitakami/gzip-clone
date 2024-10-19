#include "progress.h"
#include <stdio.h>

#define PROGRESS_BAR_WIDTH 50

void init_progress(ProgressInfo *progress, uint64_t total_bytes) {
    progress->total_bytes = total_bytes;
    progress->processed_bytes = 0;
    progress->last_percentage = -1;
    printf("Progress: [");
    fflush(stdout);
}

void update_progress(ProgressInfo *progress, uint64_t processed_bytes) {
    progress->processed_bytes += processed_bytes;
    int percentage = (int)((progress->processed_bytes * 100) / progress->total_bytes);
    
    if (percentage != progress->last_percentage) {
        int pos = PROGRESS_BAR_WIDTH * percentage / 100;
        printf("\rProgress: [");
        for (int i = 0; i < PROGRESS_BAR_WIDTH; ++i) {
            if (i < pos) printf("=");
            else if (i == pos) printf(">");
            else printf(" ");
        }
        printf("] %d%%", percentage);
        fflush(stdout);
        progress->last_percentage = percentage;
    }
}

void finish_progress(ProgressInfo *progress) {
    update_progress(progress, progress->total_bytes - progress->processed_bytes);
    printf("\n");
}
