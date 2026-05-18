/*
 * manages the tick and the overall simulation flow, including generating new planes, processing queues, and logging events.
 * The manager is responsible for coordinating the different components of the simulation and ensuring that the rules of the airport are followed.
 * It will handle the main loop of the simulation, calling the appropriate functions to process the landing and takeoff queues.
 */
#include "manager.h"
#include "../airplane/airplane.h"
#include "../utils/logger.h"
#include "../utils/settings.h"
#include "../queue/queue.h"
#include "../prioSys/prioEarly.h"
#include "../utils/misc.h"

int crashes_total = 0;
int total_emergencies = 0;

static long long total_wait_landing = 0;
static long long total_wait_takeoff = 0;
static int count_landing_processed = 0;
static int count_takeoff_processed = 0;

static int last_tick_crashes[MAX_CRASHES_PER_TICK];
static int last_tick_crash_count = 0;

int phase_one_sort_new_planes_arr(Airplane *planes) {
    const int num_new_arriving = create_new_plane_set_arriving(planes);

    ssort_fuel_basic(planes, num_new_arriving);
    return num_new_arriving;
}

int phase_one_sort_new_planes_dep(Airplane *planes) {
    const int num_new_departing = create_new_plane_set_departing(planes);

    ssort_fuel_basic(planes, num_new_departing);
    return num_new_departing;
}

bool enqueue_emergency_planes() {
    const int num_queue_1 = queue_size(&landing_queue_1);
    const int num_queue_2 = queue_size(&landing_queue_2);

    for (int i = 0; i < num_queue_1; i++) {
        Airplane plane;
        if (dequeue(&landing_queue_1, &plane)) {
            if (plane.fuel <= EMERGENCY_FUEL_THRESHOLD) {
                if (!enqueue(&emergency_queue, plane)) {
                    LOG_ERROR("Failed to enqueue emergency plane ID %d into emergency queue.", plane.id);
                } else {
                    LOG_WARNING("Enqueued emergency plane ID %d into emergency queue.", plane.id);
                }
            } else {
                if (!enqueue(&landing_queue_1, plane)) {
                    LOG_ERROR("Failed to re-enqueue plane ID %d into landing queue 2.", plane.id);
                } else {
                    LOG_INFO("Re-enqueued plane ID %d into landing queue 2.", plane.id);
                }
            }
        }
    }
    for (int i = 0; i < num_queue_2; i++) {
        Airplane plane;
        if (dequeue(&landing_queue_2, &plane)) {
            if (plane.fuel <= EMERGENCY_FUEL_THRESHOLD) {
                if (!enqueue(&emergency_queue, plane)) {
                    LOG_ERROR("Failed to enqueue emergency plane ID %d into emergency queue.", plane.id);
                } else {
                    LOG_INFO("Enqueued emergency plane ID %d into emergency queue.", plane.id);
                }
            } else {
                if (!enqueue(&landing_queue_2, plane)) {
                    LOG_ERROR("Failed to re-enqueue plane ID %d into landing queue 2.", plane.id);
                } else {
                    LOG_INFO("Re-enqueued plane ID %d into landing queue 2.", plane.id);
                }
            }
        }
    }
    return true;
}

void process_landing_queue(Queue *landing_queue, int runway_id) {
    if (is_queue_empty(landing_queue)) {
        LOG_INFO("Runway %d: No planes to land.", runway_id);
        return;
    }
    Airplane plane;
    if (dequeue(landing_queue, &plane)) {
        if (plane.type == PLANE_LANDING) {
            total_wait_landing += plane.time_in_queue;
            count_landing_processed++;
        } else {
            total_wait_takeoff += plane.time_in_queue;
            count_takeoff_processed++;
        }
        // Simulate landing process (could add more complex logic here)
        plane.status = PLANE_FINISHED;
        plane.runway = runway_id;
        LOG_INFO("Runway %d: Plane ID %d has landed successfully.", runway_id, plane.id);
    } else {
        LOG_ERROR("Runway %d: Failed to dequeue a plane for landing.", runway_id);
    }
}

/*
 * Processes the emergency queue, giving it the highest priority. If there are planes in the emergency queue, they will be processed before any other planes in the landing queues.
 * If there is only one plane in the emergency queue, it will be processed on runway 3. If there are 2-3 planes, they will be processed on runway 1 and 2. If there are more than 3 planes, it will log a warning and only process the first 3 planes, as the simulation only has 3 runways.
 * Returns the number of planes processed from the emergency queue, or -1 if there are more
 */
int pick_queue_for_emergency(const int num_emergency, int *used_queues) {
    used_queues[0] = 0;
    used_queues[1] = 0;
    used_queues[2] = 0;
    const int size_q1 = queue_size(&landing_queue_1);
    const int size_q2 = queue_size(&landing_queue_2);
    const int size_takeoff = queue_size(&takeoff_queue_1);

    if (num_emergency == 1) {
        if (size_q1 < size_takeoff || size_q2 < size_takeoff) {
            if (size_q1 < size_q2) {
                used_queues[0] = 1;
            } else {
                used_queues[0] = 2;
            }
        } else {
            used_queues[0] = 3;
        }
    } else if (num_emergency == 2) {
        if (size_q1 < size_takeoff || size_q2 < size_takeoff) {
            if (size_q1 < size_q2) {
                used_queues[0] = 1;
                if (size_q2 < size_takeoff) {
                    used_queues[1] = 2;
                } else {
                    used_queues[1] = 3;
                }
            } else {
                used_queues[0] = 2;
                if (size_q1 < size_takeoff) {
                    used_queues[1] = 1;
                } else {
                    used_queues[1] = 3;
                }
            }
        }else {
            if (size_q1 < size_q2) {
                used_queues[0] = 1;
                used_queues[1] = 3;
            } else {
                used_queues[0] = 2;
                used_queues[1] = 3;
            }
        }
    } else if (num_emergency > 2) {
        used_queues[0] = 1;
        used_queues[1] = 2;
        used_queues[2] = 3;
    }
    return 0;
}

int process_emergency_queue(int *used_emergency_lanes) {
    if (is_queue_empty(&emergency_queue)) {
        LOG_INFO("No emergency planes to process.");
        return 0;
    }
    const int num_emergency = queue_size(&emergency_queue);
    Airplane temp_planes[QUEUE_CAPACITY];
    pick_queue_for_emergency(num_emergency, used_emergency_lanes);


    LOG_WARNING("Processing %d planes in emergency queue. Assigned runways: %d, %d, %d",
        num_emergency, used_emergency_lanes[0],
        used_emergency_lanes[1], used_emergency_lanes[2]);

    for (int i = 0; i < num_emergency; i++) {
        if (!dequeue(&emergency_queue, &temp_planes[i])) {
            LOG_ERROR("Failed to dequeue plane from emergency queue during processing.");
            return -1;
        }
        if (!enqueue(&emergency_queue, temp_planes[i])) {
            LOG_ERROR("Failed to re-enqueue plane ID %d back into emergency queue during processing.", temp_planes[i].id);
            return -1;
        }
    }

    LOG_WARNING("---------->Processing emergency queue with %d planes.<-------------", num_emergency);
    for (int i = 0; i < num_emergency; i++) {
        if (i > 2) {
            LOG_WARNING("More than 3 planes in emergency queue, only processing the first 3 planes. Plane ID %d will have to wait for the next tick.", temp_planes[i].id);
            break;
        }
        process_landing_queue(&emergency_queue, used_emergency_lanes[i]);
        LOG_WARNING("Processed emergency plane ID %d on runway %d.", temp_planes[i].id, used_emergency_lanes[i]);
    }
    total_emergencies += num_emergency;
    return 0;
}

int check_crash_in_queue(Queue *q) {
    for (int i = 0; i < q->size; i++) {
        const int index = (q->head + i) % QUEUE_CAPACITY;
        if (q->data[index].fuel < CRASH_THRESHOLD) {
            q->data[index].status = PLANE_CRASHED;
            LOG_ERROR("Plane ID %d has crashed due to running out of fuel.", q->data[index].id);
            return index;
        }
    }
    return -1; // no crashes
}

static void record_crash_id(const int plane_id) {
    if (last_tick_crash_count < MAX_CRASHES_PER_TICK) {
        last_tick_crashes[last_tick_crash_count++] = plane_id;
    }
}

static void collect_crashes_from_queue(Queue *q) {
    int crash_index = check_crash_in_queue(q);
    while (crash_index != -1 && q->size > 0) {
        const int head_start = q->head;
        const int size_start = q->size;
        for (int i = 0; i < size_start; i++) {
            const int index = (head_start + i) % QUEUE_CAPACITY;
            Airplane plane;
            if (!dequeue(q, &plane)) {
                return;
            }
            if (index == crash_index) {
                record_crash_id(plane.id);
            } else if (!enqueue(q, plane)) {
                LOG_ERROR("Failed to re-enqueue plane ID %d after crash sweep.", plane.id);
            }
        }
        crash_index = check_crash_in_queue(q);
    }
}

/*
 * Processes the simulation tick on last phase of the main loop
 *  - Decrease fuel for all planes in the landing queues and emergency queue (done)
 *  - Check for any planes that have run out of fuel and remove them
 *  - remove any planes that have finished or crashed from the queues
 *  - increase time in queue for all planes in the queues
 */
void update_simulation_tick() {
    last_tick_crash_count = 0;
    // Decrease fuel for all planes in landing queues and emergency queue
    for (int i = 0; i < landing_queue_1.size; i++) {
        const int index = (landing_queue_1.head + i) % QUEUE_CAPACITY;
        landing_queue_1.data[index].fuel--;
        landing_queue_1.data[index].time_in_queue++;
    }
    for (int i = 0; i < landing_queue_2.size; i++) {
        const int index = (landing_queue_2.head + i) % QUEUE_CAPACITY;
        landing_queue_2.data[index].fuel--;
        landing_queue_2.data[index].time_in_queue++;
    }
    for (int i = 0; i < emergency_queue.size; i++) {
        const int index = (emergency_queue.head + i) % QUEUE_CAPACITY;
        emergency_queue.data[index].fuel--;
        emergency_queue.data[index].time_in_queue++;
    }

    for (int i = 0; i < takeoff_queue_1.size; i++) {
        const int index = (takeoff_queue_1.head + i) % QUEUE_CAPACITY;
        takeoff_queue_1.data[index].time_in_queue++;
    }

    collect_crashes_from_queue(&landing_queue_1);
    collect_crashes_from_queue(&landing_queue_2);
    collect_crashes_from_queue(&emergency_queue);
}

void get_average_wait_times(double *avg_landing, double *avg_takeoff) {
    if (avg_landing) {
        *avg_landing = count_landing_processed > 0
            ? (double)total_wait_landing / (double)count_landing_processed
            : 0.0;
    }
    if (avg_takeoff) {
        *avg_takeoff = count_takeoff_processed > 0
            ? (double)total_wait_takeoff / (double)count_takeoff_processed
            : 0.0;
    }
}

int get_last_tick_crashes(int *out_ids, int max_ids) {
    if (!out_ids || max_ids <= 0) {
        return 0;
    }
    const int count = last_tick_crash_count < max_ids ? last_tick_crash_count : max_ids;
    for (int i = 0; i < count; i++) {
        out_ids[i] = last_tick_crashes[i];
    }
    crashes_total += count;
    return count;
}
