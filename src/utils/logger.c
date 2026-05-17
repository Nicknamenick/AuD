#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "logger.h"
#include "../queue/queue.h"
#include "../manager/manager.h"

#define DASHBOARD_BUFFER_SIZE 256

static void clear_screen(void) {
#ifdef _WIN32
    system("cls");
#else
    fprintf(stdout, "\033[2J\033[H");
    fflush(stdout);
#endif
}

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

static void format_queue_contents(const Queue *queue, char *buffer) {
    buffer[0] = '\0';

    if (queue->size == 0) {
        snprintf(buffer, (size_t)DASHBOARD_BUFFER_SIZE, "-");
        return;
    }

    for (int i = 0; i < queue->size; i++) {
        const int index = (queue->head + i) % QUEUE_CAPACITY;
        const Airplane plane = queue->data[index];
        char entry[32];
        const int used = (int)strlen(buffer);
        const int remaining = DASHBOARD_BUFFER_SIZE - used;

        if (remaining <= 1) {
            break;
        }

        snprintf(entry, sizeof(entry), "%s%d(%d)", i > 0 ? " " : "", plane.id, plane.fuel);
        if ((int)strlen(entry) >= remaining) {
            strncat(buffer, "...", (size_t)(remaining - 1));
            break;
        }
        strncat(buffer, entry, (size_t)(DASHBOARD_BUFFER_SIZE - strlen(buffer) - 1));
    }
}

void render_dashboard(const int tick_number, const double avg_landing, const double avg_takeoff,
                      const int crash_count, const int *crash_ids
                      ,Airplane *new_planes_arr, const int num_arriving
                      ,Airplane *new_planes_dep, const int num_departing) {
    clear_screen();
    const char *border = "+-----------------------------------------------------------------------------------+";
    char q1_buffer[DASHBOARD_BUFFER_SIZE];
    char q2_buffer[DASHBOARD_BUFFER_SIZE];
    char emergency_buffer[DASHBOARD_BUFFER_SIZE];
    char takeoff_buffer[DASHBOARD_BUFFER_SIZE];
    char new_arr_buffer[DASHBOARD_BUFFER_SIZE];
    char new_dep_buffer[DASHBOARD_BUFFER_SIZE];
    Queue temp_q1, temp_q2;

    init_queue(&temp_q1);
    init_queue(&temp_q2);
    enqueue_arr(&temp_q1, new_planes_arr, num_arriving);
    enqueue_arr(&temp_q2, new_planes_dep, num_departing);

    format_queue_contents(&landing_queue_1, q1_buffer);
    format_queue_contents(&landing_queue_2, q2_buffer);
    format_queue_contents(&emergency_queue, emergency_buffer);
    format_queue_contents(&takeoff_queue_1, takeoff_buffer);
    format_queue_contents(&temp_q1, new_arr_buffer);
    format_queue_contents(&temp_q2, new_dep_buffer);

    fprintf(stdout, "%s\n", border);
    fprintf(stdout, "| AIRPORT SIMULATION DASHBOARD                                                      |\n");
    fprintf(stdout, "| Tick: %-75d |\n", tick_number);
    fprintf(stdout, "%s\n", border);
    fprintf(stdout, "| %-18s | %5d | %-52s |\n", "new_planes_arr", queue_size(&temp_q1), new_arr_buffer);
    fprintf(stdout, "| %-18s | %5d | %-52s |\n", "new_planes_dep", queue_size(&temp_q2), new_dep_buffer);
    fprintf(stdout, "%s\n", border);
    fprintf(stdout, "| %-18s | %-5s | %-52s |\n", "Queue", "Size", "Contents (PlaneID(Fuel)))");
    fprintf(stdout, "+--------------------+-------+------------------------------------------------------+\n");
    fprintf(stdout, "| %-18s | %5d | %-52s |\n", "landing_queue_1", queue_size(&landing_queue_1), q1_buffer);
    fprintf(stdout, "| %-18s | %5d | %-52s |\n", "landing_queue_2", queue_size(&landing_queue_2), q2_buffer);
    fprintf(stdout, "| %-18s | %5d | %-52s |\n", "emergency_queue", queue_size(&emergency_queue), emergency_buffer);
    fprintf(stdout, "| %-18s | %5d | %-52s |\n", "takeoff_queue_1", queue_size(&takeoff_queue_1), takeoff_buffer);
    fprintf(stdout, "+--------------------+-------+------------------------------------------------------+\n");
    fprintf(stdout, "| %-18s | %5.2f | %-52s |\n", "avg_wait_landing", avg_landing, "Average waiting time for landings");
    fprintf(stdout, "| %-18s | %5.2f | %-52s |\n", "avg_wait_takeoff", avg_takeoff, "Average waiting time for takeoffs");
    fprintf(stdout, "+--------------------+-------+------------------------------------------------------+\n");

    if (crash_count == 0) {
        fprintf(stdout, "| %-18s | %5d | %-52s |\n", "crashes_last_tick", 0, "none");
    } else {
        char crash_buffer[256];
        crash_buffer[0] = '\0';
        for (int i = 0; i < crash_count; i++) {
            char id_buffer[16];
            snprintf(id_buffer, sizeof(id_buffer), "%d%s", crash_ids[i], i < crash_count - 1 ? "," : "");
            strncat(crash_buffer, id_buffer, sizeof(crash_buffer) - strlen(crash_buffer) - 1);
            if (strlen(crash_buffer) >= sizeof(crash_buffer) - 4) {
                strncat(crash_buffer, "...", sizeof(crash_buffer) - strlen(crash_buffer) - 1);
                break;
            }
        }
        fprintf(stdout, "| %-18s | %5d | %-52s |\n", "crashes_last_tick", crash_count, crash_buffer);
    }
    //total crashes
    fprintf(stdout, "| %-18s | %5d | %-52s |\n", "crashes_total", crashes_total, "Total crashes so far");
    fprintf(stdout, "%s\n", border);
    fflush(stdout);
}
