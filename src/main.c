#include <stdio.h>
#include "utils/logger.h"
#include "airplane/airplane.h"
#include "queue/queue.h"
#include "manager/manager.h"
#include "prioSys/prioQueue.h"

PrioQueueConfig prio_config;

// TODO - log finished or crashed planes and remove them from the queues in process_simulation_tick, instead of just decreasing fuel and collecting crashes. This will prevent the queues from filling up with crashed planes that are still taking up space and affecting prioritization.

int main(void) {
    init_queue(&landing_queue_1);
    init_queue(&landing_queue_2);
    init_queue(&takeoff_queue_1);
    init_prio_queue_config(&prio_config);


    LOG_INFO("Initialized queues: landing_queue_1, landing_queue_2, takeoff_queue_1");
    for (int i = 0; i < 100; i++) {
        //phase 1 -> new planes
        Airplane new_planes_arr[UPPER_PLANE_ARRIVAL_RATE];
        Airplane new_planes_dep[UPPER_PLANE_DEPARTURE_RATE];

        const int num_arriving = phase_one_sort_new_planes_arr(new_planes_arr);
        const int num_departing = phase_one_sort_new_planes_dep(new_planes_dep);

        //phase 2 -> set priorities and enqueue new planes
        enqueue_landing_with_prio(new_planes_arr, num_arriving, &landing_queue_1, &landing_queue_2, &prio_config);
        enqueue_emergency_planes();
        enqueue_arr(&takeoff_queue_1, new_planes_dep, num_departing);

        //phase 3 -> processing queues
        check_crash_in_queue(&landing_queue_1);
        check_crash_in_queue(&landing_queue_2);
        check_crash_in_queue(&emergency_queue);
        const int emergency_code = process_emergency_queue();

        if (emergency_code == -1) {
            LOG_ERROR("Invalid emergency code returned from process_emergency_queue: %d", emergency_code);
            return 0;
        }
        if (emergency_code == 0) {
            process_landing_queue(&landing_queue_1, 1);
            process_landing_queue(&landing_queue_2, 2);
            process_landing_queue(&takeoff_queue_1, 3);
        }
        if (emergency_code == 3) {
            process_landing_queue(&landing_queue_1, 1);
            process_landing_queue(&landing_queue_2, 2);
        }
        if (emergency_code == 2) {
            process_landing_queue(&landing_queue_1, 1);
        }
        if (emergency_code == 1) {
            LOG_WARNING("!!!!all runways are occupied with emergency landings, skipping normal landing processing for this tick.!!!!");
        }

        process_simulation_tick();

        double avg_landing = 0.0;
        double avg_takeoff = 0.0;
        get_average_wait_times(&avg_landing, &avg_takeoff);


        print_queue(&landing_queue_1);
        print_queue(&landing_queue_2);
        print_queue(&emergency_queue);
        print_queue(&takeoff_queue_1);

        printf("\n");
        LOG_INFO("QUEUE SIZES: landing_queue_1: %d, landing_queue_2: %d, emergency_queue: %d, takeoff_queue_1: %d",
                 queue_size(&landing_queue_1), queue_size(&landing_queue_2),
                 queue_size(&emergency_queue), queue_size(&takeoff_queue_1));
        printf("\n");
        LOG_INFO("AVG WAIT: landing=%.2f, takeoff=%.2f", avg_landing, avg_takeoff);

        int crash_ids[QUEUE_CAPACITY * 3];
        const int crash_count = get_last_tick_crashes(crash_ids, QUEUE_CAPACITY * 3);
        if (crash_count == 0) {
            LOG_INFO("CRASHES LAST TICK: none");
        } else {
            LOG_WARNING("CRASHES LAST TICK (count: %d):", crash_count);
            for (int j = 0; j < crash_count; j++) {
                LOG_INFO("  Plane ID: %d", crash_ids[j]);
            }
        }
        LOG_INFO("---------------------------------------- End of tick %d ----------------------------------------", i + 1);
    }
    return 0;
}
