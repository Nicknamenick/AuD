#ifndef SIM_LOGGER_H
#define SIM_LOGGER_H

typedef enum {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
} logLevel;

void log_message(logLevel level, const char *format, ...);

#include "settings.h"
#if VERBOSE == 1
    #define LOG_INFO(format, ...) log_message(LOG_INFO, format, ##__VA_ARGS__)
    #define LOG_WARNING(format, ...) log_message(LOG_WARNING, format, ##__VA_ARGS__)
    #define LOG_ERROR(format, ...) log_message(LOG_ERROR, format, ##__VA_ARGS__)
#else
    #define LOG_INFO(format, ...)
    #define LOG_WARNING(format, ...)
    #define LOG_ERROR(format, ...)
#endif
#endif