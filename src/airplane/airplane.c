#include "airplane.h"

#include "../utils/logger.h"
#include "../utils/settings.h"
#include "../utils/misc.h"


/* get a surrogate key for a new plane */
int get_new_plane_id() {
    static int id_counter = 1;
    return id_counter++;
}

/*
 * Creates a new set of planes arriving at the current tick.
 * The number of planes is randomly determined between UPPER_PLANE_RATE and LOWER_PLANE_RATE.
 * There can't be more than 3 planes arriving at the same tick.
 * Fuel is randomly assigned between LOWER_FUEL_LIMIT and UPPER_FUEL_LIMIT.
 */
int create_new_plane_set_arriving(Airplane *plane_set) {
    const int num_planes = (int)(rand_xor() % (UPPER_PLANE_ARRIVAL_RATE - LOWER_PLANE_ARRIVAL_RATE + 1)) + LOWER_PLANE_ARRIVAL_RATE;
    for (int i = 0; i < num_planes; i++) {
        plane_set[i].id = get_new_plane_id();
        plane_set[i].type = PLANE_LANDING;
        plane_set[i].status = PLANE_WAITING;
        plane_set[i].fuel = LOWER_FUEL_LIMIT + (int)(rand_xor() % (UPPER_FUEL_LIMIT - LOWER_FUEL_LIMIT + 1));
        plane_set[i].time_in_queue = 0;
        plane_set[i].runway = -1;
        plane_set[i].time_processed = 0;
    }
    LOG_INFO("Generated %d new arriving planes.", num_planes);
    return num_planes;
}

int create_new_plane_set_departing(Airplane *plane_set) {
    const int num_planes = (int)(rand_xor() % (UPPER_PLANE_DEPARTURE_RATE - LOWER_PLANE_DEPARTURE_RATE + 1)) + LOWER_PLANE_DEPARTURE_RATE;
    for (int i = 0; i < num_planes; i++) {
        plane_set[i].id = get_new_plane_id();
        plane_set[i].type = PLANE_TAKEOFF;
        plane_set[i].status = PLANE_WAITING;
        plane_set[i].fuel = 999; // Takeoff planes have unlimited fuel
        plane_set[i].time_in_queue = 0;
        plane_set[i].runway = -1;
        plane_set[i].time_processed = 0;
    }
    LOG_INFO("Generated %d new departing planes.", num_planes);
    return num_planes;
}

void print_airplane(const Airplane *plane) {
    LOG_INFO("Plane ID: %d, Type: %s, Status: %s, Fuel: %d, Time in Queue: %d, Runway: %d, Time Processed: %d",
           plane->id,
           plane->type == PLANE_LANDING ? "Landing" : "Takeoff",
           plane->status == PLANE_WAITING ? "Waiting" :
           plane->status == PLANE_QUEUED ? "Queued" :
           plane->status == PLANE_FINISHED ? "Finished" : "Crashed",
           plane->fuel,
           plane->time_in_queue,
           plane->runway,
           plane->time_processed);
}
