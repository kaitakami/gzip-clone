#include "help.h"
#include <stdio.h>

void display_help() {
    printf("GZIP-Clone: File Compression CLI Tool in C\n\n");
    printf("Usage: gzip-clone [OPTIONS] [FILES]\n\n");
    printf("Options:\n");
    printf("  -c, --compress            Compress the specified files (default action).\n");
    printf("  -d, --decompress          Decompress the specified .gz files.\n");
    printf("  -l, --level [1-9]         Set compression level (1 = fastest, 9 = best compression).\n");
    printf("  -o, --output [FILE/DIR]   Specify output file or directory.\n");
    printf("  -r, --recursive           Recursively compress/decompress files in directories.\n");
    printf("  -t, --threads [NUM]       Set the number of threads to use.\n");
    printf("  -h, --help                Display this help message and exit.\n");
    printf("  -v, --version             Display version information and exit.\n");
}
