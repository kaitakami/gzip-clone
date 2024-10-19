#ifndef DECOMPRESS_H
#define DECOMPRESS_H

#include "config.h"

int decompress_file_stream(const char *input_file, const char *output_file, Config *config);
int decompress_file(const char *input_file, const char *output_file, Config *config);

#endif // DECOMPRESS_H
