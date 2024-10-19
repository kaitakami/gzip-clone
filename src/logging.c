#include "logging.h"
#include <stdarg.h>
#include <time.h>
#include <string.h>

static FILE *log_file = NULL;
static LogLevel current_level = LOG_INFO;

void init_logger(const char *filename, LogLevel level) {
    if (log_file) {
        fclose(log_file);
    }
    log_file = fopen(filename, "a");
    if (!log_file) {
        fprintf(stderr, "Failed to open log file: %s\n", filename);
        return;
    }
    current_level = level;
}

void close_logger(void) {
    if (log_file) {
        fclose(log_file);
        log_file = NULL;
    }
}

void log_message(LogLevel level, const char *file, int line, const char *format, ...) {
    if (level < current_level) return;

    const char *level_strings[] = {"DEBUG", "INFO", "WARN", "ERROR"};
    
    time_t now;
    time(&now);
    char time_buf[sizeof "2011-10-08T07:07:09Z"];
    strftime(time_buf, sizeof time_buf, "%FT%TZ", gmtime(&now));

    fprintf(log_file ? log_file : stderr, "[%s] [%s] %s:%d: ", 
            time_buf, level_strings[level], file, line);

    va_list args;
    va_start(args, format);
    vfprintf(log_file ? log_file : stderr, format, args);
    va_end(args);
    fprintf(log_file ? log_file : stderr, "\n");
    fflush(log_file ? log_file : stderr);
}
