#include "decompress.h"
#include "file_utils.h"
#include "logging.h"
#include "progress.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#define CHUNK 16384

int decompress_file_stream(const char *input_file, const char *output_file, Config *config) {
    (void)config;  // Mark as unused
    FILE *source = fopen(input_file, "rb");
    FILE *dest = fopen(output_file, "wb");
    
    if (!source || !dest) {
        if (source) fclose(source);
        if (dest) fclose(dest);
        LOG_ERROR("Failed to open files for decompression");
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
    strm.avail_in = 0;
    strm.next_in = Z_NULL;

    if (inflateInit2(&strm, 47) != Z_OK) {
        LOG_ERROR("Failed to initialize zlib for decompression");
        fclose(source);
        fclose(dest);
        return -1;
    }

    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    int ret;
    uLong crc = crc32(0L, Z_NULL, 0);
    long total_out = 0;

    do {
        strm.avail_in = fread(in, 1, CHUNK, source);
        update_progress(&progress, strm.avail_in);
        if (ferror(source)) {
            LOG_ERROR("Error reading input file during decompression");
            inflateEnd(&strm);
            fclose(source);
            fclose(dest);
            return -1;
        }
        if (strm.avail_in == 0)
            break;
        strm.next_in = in;

        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);
            switch (ret) {
            case Z_NEED_DICT:
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                LOG_ERROR("Zlib error during decompression");
                inflateEnd(&strm);
                fclose(source);
                fclose(dest);
                return -1;
            }
            unsigned have = CHUNK - strm.avail_out;
            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
                LOG_ERROR("Error writing output file during decompression");
                inflateEnd(&strm);
                fclose(source);
                fclose(dest);
                return -1;
            }
            crc = crc32(crc, out, have);
            total_out += have;
        } while (strm.avail_out == 0);
    } while (ret != Z_STREAM_END);

    finish_progress(&progress);

    inflateEnd(&strm);

    // Read CRC32 and original size from the end of the file
    uLong stored_crc;
    long stored_size;
    fseek(source, -sizeof(uLong)-sizeof(long), SEEK_END);
    fread(&stored_crc, sizeof(uLong), 1, source);
    fread(&stored_size, sizeof(long), 1, source);

    fclose(source);
    fclose(dest);

    if (crc != stored_crc || total_out != stored_size) {
        LOG_ERROR("CRC check failed or file size mismatch");
        return -1;
    }

    set_file_attributes(output_file, &attr);

    LOG_INFO("Successfully decompressed file: %s", input_file);
    return ret == Z_STREAM_END ? 0 : -1;
}

int decompress_file(const char *input_file, const char *output_file, Config *config) {
    return decompress_file_stream(input_file, output_file, config);
}
