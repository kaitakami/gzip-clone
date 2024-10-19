#define _POSIX_C_SOURCE 200809L
#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <inttypes.h>
#include "../include/compress.h"
#include "../include/decompress.h"
#include "../include/config.h"
#include "../include/cli.h"
#include "../include/directory.h"

// Test case for compression
START_TEST(test_compress)
{
    Config config;
    init_config(&config);
    config.compression_level = 6;

    const char *input_file = "test_input.txt";
    const char *output_file = "test_output.gz";

    // Create a larger, more compressible test input file
    FILE *f = fopen(input_file, "w");
    for (int i = 0; i < 1000; i++) {
        fprintf(f, "This is a test file with repetitive content for better compression.\n");
    }
    fclose(f);

    // Compress the file
    int result = compress_file(input_file, output_file, &config);
    ck_assert_int_eq(result, 0);

    // Check if the output file exists and is smaller than the input
    struct stat input_stat, output_stat;
    stat(input_file, &input_stat);
    stat(output_file, &output_stat);
    
    printf("Input file size: %" PRId64 " bytes\n", (int64_t)input_stat.st_size);
    printf("Output file size: %" PRId64 " bytes\n", (int64_t)output_stat.st_size);
    
    ck_assert(output_stat.st_size > 0);
    ck_assert_msg(output_stat.st_size < input_stat.st_size, 
                  "Compressed file (%" PRId64 " bytes) is not smaller than input file (%" PRId64 " bytes)",
                  (int64_t)output_stat.st_size, (int64_t)input_stat.st_size);

    // Clean up
    remove(input_file);
    remove(output_file);
}
END_TEST

// Test case for decompression
START_TEST(test_decompress)
{
    Config config;
    init_config(&config);

    const char *input_file = "test_input.gz";
    const char *output_file = "test_output.txt";
    const char *original_content = "This is a test file for decompression.\n";

    // Create a test compressed file
    FILE *f = fopen("temp.txt", "w");
    fprintf(f, "%s", original_content);
    fclose(f);
    compress_file("temp.txt", input_file, &config);
    remove("temp.txt");

    // Decompress the file
    int result = decompress_file(input_file, output_file, &config);
    ck_assert_int_eq(result, 0);

    // Check if the output file exists and has the correct content
    FILE *f_out = fopen(output_file, "r");
    char buffer[100];
    fgets(buffer, sizeof(buffer), f_out);
    fclose(f_out);
    ck_assert_str_eq(buffer, original_content);

    // Clean up
    remove(input_file);
    remove(output_file);
}
END_TEST

// Add this new test case
START_TEST(test_large_file_compression)
{
    Config config;
    init_config(&config);
    config.compression_level = 6;

    const char *input_file = "large_test_input.txt";
    const char *output_file = "large_test_output.gz";

    // Create a large test input file (100 MB)
    FILE *f = fopen(input_file, "w");
    for (int i = 0; i < 1024 * 1024 * 100 / 100; i++) {
        fprintf(f, "This is a test file with repetitive content for better compression. Line %d\n", i);
    }
    fclose(f);

    // Compress the file
    int result = compress_file_stream(input_file, output_file, &config);
    ck_assert_int_eq(result, 0);

    // Check if the output file exists and is smaller than the input
    struct stat input_stat, output_stat;
    stat(input_file, &input_stat);
    stat(output_file, &output_stat);
    
    printf("Large input file size: %" PRId64 " bytes\n", (int64_t)input_stat.st_size);
    printf("Large output file size: %" PRId64 " bytes\n", (int64_t)output_stat.st_size);
    
    ck_assert(output_stat.st_size > 0);
    ck_assert_msg(output_stat.st_size < input_stat.st_size, 
                  "Compressed file (%" PRId64 " bytes) is not smaller than input file (%" PRId64 " bytes)",
                  (int64_t)output_stat.st_size, (int64_t)input_stat.st_size);

    // Clean up
    remove(input_file);
    remove(output_file);
}
END_TEST

START_TEST(test_compression_levels)
{
    Config config;
    init_config(&config);

    const char *input_file = "test_compression_levels.txt";
    const char *output_file_prefix = "test_compression_levels_";
    
    // Create a test file
    FILE *f = fopen(input_file, "w");
    for (int i = 0; i < 10000; i++) {
        fprintf(f, "This is a test file for compression levels.\n");
    }
    fclose(f);

    struct stat input_stat;
    stat(input_file, &input_stat);

    for (int level = 1; level <= 9; level++) {
        config.compression_level = level;
        char output_file[100];
        snprintf(output_file, sizeof(output_file), "%s%d.gz", output_file_prefix, level);

        int result = compress_file(input_file, output_file, &config);
        ck_assert_int_eq(result, 0);

        struct stat output_stat;
        stat(output_file, &output_stat);

        printf("Compression level %d: Input size: %" PRId64 ", Output size: %" PRId64 "\n", 
               level, (int64_t)input_stat.st_size, (int64_t)output_stat.st_size);

        ck_assert(output_stat.st_size > 0);
        ck_assert(output_stat.st_size < input_stat.st_size);

        remove(output_file);
    }

    remove(input_file);
}
END_TEST

START_TEST(test_recursive_compression)
{
    Config config;
    init_config(&config);
    config.recursive = 1;

    // Create a test directory structure
    mkdir("test_recursive", 0777);
    mkdir("test_recursive/subdir1", 0777);
    mkdir("test_recursive/subdir2", 0777);

    FILE *f1 = fopen("test_recursive/file1.txt", "w");
    fprintf(f1, "File 1 content\n");
    fclose(f1);

    FILE *f2 = fopen("test_recursive/subdir1/file2.txt", "w");
    fprintf(f2, "File 2 content\n");
    fclose(f2);

    FILE *f3 = fopen("test_recursive/subdir2/file3.txt", "w");
    fprintf(f3, "File 3 content\n");
    fclose(f3);

    // Compress recursively
    char *argv[] = {"gzip-clone", "-c", "-r", "test_recursive"};
    int argc = 4;
    parse_arguments(argc, argv, &config);

    int result = process_directory("test_recursive", &config);
    ck_assert_int_eq(result, 0);

    // Check if compressed files exist
    ck_assert_int_eq(access("test_recursive/file1.txt.gz", F_OK), 0);
    ck_assert_int_eq(access("test_recursive/subdir1/file2.txt.gz", F_OK), 0);
    ck_assert_int_eq(access("test_recursive/subdir2/file3.txt.gz", F_OK), 0);

    // Clean up
    remove("test_recursive/file1.txt.gz");
    remove("test_recursive/subdir1/file2.txt.gz");
    remove("test_recursive/subdir2/file3.txt.gz");
    remove("test_recursive/file1.txt");
    remove("test_recursive/subdir1/file2.txt");
    remove("test_recursive/subdir2/file3.txt");
    rmdir("test_recursive/subdir1");
    rmdir("test_recursive/subdir2");
    rmdir("test_recursive");
}
END_TEST

// Create test suite
Suite *gzip_clone_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("GZIP-Clone");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_compress);
    tcase_add_test(tc_core, test_decompress);
    tcase_add_test(tc_core, test_large_file_compression);
    tcase_add_test(tc_core, test_compression_levels);
    tcase_add_test(tc_core, test_recursive_compression);
    suite_add_tcase(s, tc_core);

    return s;
}

// Main function to run the tests
int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = gzip_clone_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
