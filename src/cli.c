#include "cli.h"
#include "help.h"
#include "version.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

void parse_arguments(int argc, char *argv[], Config *config) {
    int opt;
    int option_index = 0;

    static struct option long_options[] = {
        {"compress",   no_argument,       0, 'c'},
        {"decompress", no_argument,       0, 'd'},
        {"level",      required_argument, 0, 'l'},
        {"output",     required_argument, 0, 'o'},
        {"recursive",  no_argument,       0, 'r'},
        {"threads",    required_argument, 0, 't'},
        {"help",       no_argument,       0, 'h'},
        {"version",    no_argument,       0, 'v'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "cdl:o:rt:hv", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'c':
                config->compress = 1;
                break;
            case 'd':
                config->decompress = 1;
                break;
            case 'l':
                config->compression_level = atoi(optarg);
                if(config->compression_level < 1 || config->compression_level > 9) {
                    fprintf(stderr, "Invalid compression level: %d. Must be between 1 and 9.\n", config->compression_level);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'o':
                config->output_path = strdup(optarg);
                break;
            case 'r':
                config->recursive = 1;
                break;
            case 't':
                config->threads = atoi(optarg);
                if(config->threads < 1) {
                    fprintf(stderr, "Invalid number of threads: %d. Must be at least 1.\n", config->threads);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'h':
                display_help();
                exit(EXIT_SUCCESS);
                break;
            case 'v':
                display_version();
                exit(EXIT_SUCCESS);
                break;
            default:
                fprintf(stderr, "Unknown option encountered.\n");
                exit(EXIT_FAILURE);
                break;
        }
    }

    // Handle non-option arguments (e.g., file names)
    config->file_count = argc - optind;
    if(config->file_count > 0) {
        config->files = &argv[optind];
    } else {
        fprintf(stderr, "No input files specified.\n");
        exit(EXIT_FAILURE);
    }
}
