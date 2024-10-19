#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
    int compress;
    int decompress;
    int compression_level;
    char *output_path;
    int recursive;
    int threads;
    char **files;
    int file_count;
} Config;

void init_config(Config *config);

#endif // CONFIG_H
