#include "config.h"
#include <stdlib.h>

void init_config(Config *config) {
    config->compress = 0;
    config->decompress = 0;
    config->compression_level = 5; // Default compression level
    config->output_path = NULL;
    config->recursive = 0;
    config->threads = 1; // Default to single thread
    config->files = NULL;
    config->file_count = 0;
}
