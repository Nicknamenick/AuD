/*
 * manages the tick and the overall simulation flow, including generating new planes, processing queues, and logging events.
 * The manager is responsible for coordinating the different components of the simulation and ensuring that the rules of the airport are followed.
 * It will handle the main loop of the simulation, calling the appropriate functions to process the landing and takeoff queues.
 */

#include "manager.h"
#include "../airplane/airplane.h"
#include "../queue/queue.h"
#include "../utils/logger.h"

// TODO - implement prios (next thing)
bool phase_one_enqueue_new_planes() {
    Airplane new_arriving_planes[UPPER_PLANE_ARRIVAL_RATE];
    Airplane new_departing_planes[UPPER_PLANE_DEPARTURE_RATE];
    const int num_new_arriving = create_new_plane_set_arriving(new_arriving_planes);
    const int num_new_departing = create_new_plane_set_departing(new_departing_planes);

    enqueue_arr(&landing_queue_1, new_arriving_planes, num_new_arriving);
    enqueue_arr(&takeoff_queue_1, new_departing_planes, num_new_departing);

    print_queue(&landing_queue_1);
    print_queue(&takeoff_queue_1);

    return true;
}