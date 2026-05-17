#ifndef SIM_MANAGER_H
#define SIM_MANAGER_H
#include "../airplane/airplane.h"
#include <stdbool.h>
#include "../queue/queue.h"

int phase_one_sort_new_planes_arr(Airplane *planes);
int phase_one_sort_new_planes_dep(Airplane *planes);

bool enqueue_emergency_planes();

int check_crash_in_queue(Queue *q);
void process_landing_queue(Queue *landing_queue, int runway_id);
int process_emergency_queue();

void process_simulation_tick();

void get_average_wait_times(double *avg_landing, double *avg_takeoff);
int get_last_tick_crashes(int *out_ids, int max_ids);

#endif