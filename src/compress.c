#include "compress.h"
#include "file_utils.h"
#include "logging.h"
#include "progress.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#define CHUNK 16384

int compress_file_stream(const char *input_file, const char *output_file, Config *config) {
    FILE *source = fopen(input_file, "rb");
    FILE *dest = fopen(output_file, "wb");
    
    if (!source || !dest) {
        if (source) fclose(source);
        if (dest) fclose(dest);
        LOG_ERROR("Failed to open files for compression");
        return -1;
    }

    FileAttributes attr;
    get_file_attributes(input_file, &attr);

    // Get file size for progress reporting
    fseek(source, 0L, SEEK_END);
    long file_size = ftell(source);
    rewind(source);

    ProgressInfo progress;
    init_progress(&progress, file_size);

    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;

    if (deflateInit2(&strm, config->compression_level, Z_DEFLATED, 15 | 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
        LOG_ERROR("Failed to initialize zlib for compression");
        fclose(source);
        fclose(dest);
        return -1;
    }

    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    int flush;
    int ret;
    uLong crc = crc32(0L, Z_NULL, 0);

    do {
        strm.avail_in = fread(in, 1, CHUNK, source);
        update_progress(&progress, strm.avail_in);
        if (ferror(source)) {
            LOG_ERROR("Error reading input file during compression");
            deflateEnd(&strm);
            fclose(source);
            fclose(dest);
            return -1;
        }
        crc = crc32(crc, in, strm.avail_in);
        flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = in;

        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = deflate(&strm, flush);
            if (ret == Z_STREAM_ERROR) {
                LOG_ERROR("Zlib error during compression");
                deflateEnd(&strm);
                fclose(source);
                fclose(dest);
                return -1;
            }
            unsigned have = CHUNK - strm.avail_out;
            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
                LOG_ERROR("Error writing output file during compression");
                deflateEnd(&strm);
                fclose(source);
                fclose(dest);
                return -1;
            }
        } while (strm.avail_out == 0);
    } while (flush != Z_FINISH);

    deflateEnd(&strm);

    // Write CRC32 and input size to the end of the file
    fwrite(&crc, sizeof(uLong), 1, dest);
    fwrite(&file_size, sizeof(long), 1, dest);

    finish_progress(&progress);

    fclose(source);
    fclose(dest);

    set_file_attributes(output_file, &attr);

    LOG_INFO("Successfully compressed file: %s (CRC: %lu, Size: %ld)", input_file, crc, file_size);
    return 0;
}

int compress_file(const char *input_file, const char *output_file, Config *config) {
    return compress_file_stream(input_file, output_file, config);
}

int verify_integrity(const char *input_file) {
    FILE *source = fopen(input_file, "rb");
    if (!source) {
        LOG_ERROR("Failed to open file for integrity check: %s", input_file);
        return -1;
    }

    // Get file size
    fseek(source, 0L, SEEK_END);
    long file_size = ftell(source);
    rewind(source);

    if ((long long)file_size < (long long)(sizeof(uLong) + sizeof(long))) {
        LOG_ERROR("File is too small to contain valid CRC and size information: %s (size: %lld bytes)", input_file, (long long)file_size);
        fclose(source);
        return -1;
    }

    // Read stored CRC32 and original size
    uLong stored_crc;
    long stored_size;
    fseek(source, -sizeof(uLong)-sizeof(long), SEEK_END);
    size_t read_crc = fread(&stored_crc, sizeof(uLong), 1, source);
    size_t read_size = fread(&stored_size, sizeof(long), 1, source);
    
    if (read_crc != 1 || read_size != 1) {
        LOG_ERROR("Failed to read CRC or size information from file: %s", input_file);
        fclose(source);
        return -1;
    }

    // Reset file pointer to beginning
    rewind(source);

    // Calculate CRC32 of compressed data
    uLong calculated_crc = crc32(0L, Z_NULL, 0);
    unsigned char buffer[CHUNK];
    size_t bytes_read;
    long total_read = 0;
    long data_size = file_size - sizeof(uLong) - sizeof(long);

    while (total_read < data_size) {
        bytes_read = fread(buffer, 1, (data_size - total_read < CHUNK) ? (data_size - total_read) : CHUNK, source);
        if (bytes_read == 0) {
            if (ferror(source)) {
                LOG_ERROR("Error reading file during integrity check: %s", input_file);
                fclose(source);
                return -1;
            }
            break;
        }
        calculated_crc = crc32(calculated_crc, buffer, bytes_read);
        total_read += bytes_read;
    }

    fclose(source);

    if (calculated_crc != stored_crc) {
        LOG_ERROR("CRC check failed for file: %s (Calculated: %lu, Stored: %lu)", input_file, calculated_crc, stored_crc);
        return -1;
    }

    LOG_INFO("Integrity check passed for file: %s (CRC: %lu, Size: %ld)", input_file, stored_crc, stored_size);
    return 0;
}
