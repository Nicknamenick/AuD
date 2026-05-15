#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include "logger.h"

/* Converts log level to string (INFO, WARNING, ERROR) */
static const char *level_string(const logLevel level) {
    switch (level) {
        case LOG_INFO: return "INFO";
        case LOG_WARNING: return "WARNING";
        case LOG_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

/* Formats log level as [LEVEL_NAME] */
static const char *log_level(const logLevel level) {
    const char *level_str = level_string(level);
    static char buffer[64];
    snprintf(buffer, sizeof(buffer), "[%s]", level_str);
    return buffer;
}

/* Outputs a formatted log message with timestamp and level */
void log_message(const logLevel level, const char *format, ...) {
    const time_t now = time(NULL);
    const struct tm *tm_info = localtime(&now);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%H:%M:%S", tm_info);

    const char *level_str = log_level(level);
    va_list args;
    va_start(args, format);
    fprintf(stdout, "%s %s ", time_str, level_str);
    vfprintf(stdout, format, args);
    va_end(args);
    fprintf(stdout, "\n");
}