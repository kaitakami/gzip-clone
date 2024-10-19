#ifndef COMPRESS_H
#define COMPRESS_H

#include "config.h"

int compress_file_stream(const char *input_file, const char *output_file, Config *config);
int compress_file(const char *input_file, const char *output_file, Config *config);

#endif // COMPRESS_H
