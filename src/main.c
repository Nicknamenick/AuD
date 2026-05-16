#include "utils/logger.h"
#include "airplane/airplane.h"
#include "queue/queue.h"
#include "manager/manager.h"

void testQueue(void);

int main(void)
{
    init_queue(&landing_queue_1);
    init_queue(&landing_queue_2);
    init_queue(&takeoff_queue_1);
    LOG_INFO("Initialized queues: landing_queue_1, landing_queue_2, takeoff_queue_1");

    phase_one_enqueue_new_planes();

    return 0;
}


void testQueue() {
    Airplane new_set[3];
    int num_planes = create_new_plane_set_arriving(new_set);
    LOG_INFO("Created %d new planes arriving:", num_planes);
    for (int i = 0; i < num_planes; i++) {
        LOG_INFO("Plane ID: %d, Type: %s, Status: %s, Fuel: %d",
               new_set[i].id,
               new_set[i].type == PLANE_LANDING ? "Landing" : "Takeoff",
               new_set[i].status == PLANE_WAITING ? "Waiting" :
               new_set[i].status == PLANE_QUEUED ? "Queued" :
               new_set[i].status == PLANE_FINISHED ? "Finished" : "Crashed",
               new_set[i].fuel);
        if (!enqueue(&landing_queue_1, new_set[i])) {
            LOG_ERROR("Failed to enqueue plane ID %d into landing_queue_1", new_set[i].id);
        } else {
            LOG_INFO("Enqueued plane ID %d into landing_queue_1", new_set[i].id);
        }
    }
    LOG_INFO("Current state of landing_queue_1 after enqueuing new planes:");
    print_queue(&landing_queue_1);
}