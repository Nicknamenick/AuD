#ifndef SIM_LOGGER_H
#define SIM_LOGGER_H

#include "../airplane/airplane.h"

typedef enum {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
} logLevel;

void log_message(logLevel level, const char *format, ...);
void render_dashboard(int tick_number, double avg_landing,
    double avg_takeoff, int crash_count, const int *crash_ids
    ,Airplane *new_planes_arr, int num_arriving,
    Airplane *new_planes_dep, int num_departing);

#define LOG_ERROR(format, ...) log_message(LOG_ERROR, format, ##__VA_ARGS__)
#define LOG_WARNING(format, ...) log_message(LOG_WARNING, format, ##__VA_ARGS__)


#include "settings.h"
#if VERBOSE == 1
    #define LOG_INFO(format, ...) log_message(LOG_INFO, format, ##__VA_ARGS__)
#else
    #define LOG_INFO(format, ...)
#endif
#endif