#include <stdio.h>
#include "utils/logger.h"
#include "airplane/airplane.h"
#include "queue/queue.h"

int main(void)
{
    printf("logger test\n");
    LOG_INFO("This is an info message. %s", "Hello, World!");
    LOG_WARNING("This is a warning message. %d", 42);
    LOG_ERROR("This is an error message. %f", 3.14);

    int num_planes = 5;
    Airplane planes[5] = {
        {get_new_plane_id(), PLANE_LANDING, PLANE_WAITING, 100, false, 0, -1, 0},
        {get_new_plane_id(), PLANE_TAKEOFF, PLANE_WAITING, 80, false, 0, -1, 0},
        {get_new_plane_id(), PLANE_LANDING, PLANE_WAITING, 120, true, 0, -1, 0},
        {get_new_plane_id(), PLANE_TAKEOFF, PLANE_WAITING, 90, false, 0, -1, 0},
        {get_new_plane_id(), PLANE_LANDING, PLANE_WAITING, 110, false, 0, -1, 0}
    };

    Queue landing_queue;
    init_queue(&landing_queue);
    for (int i = 0; i < num_planes; i++) {
        if (enqueue(&landing_queue, planes[i])) {
            LOG_INFO("Enqueued plane ID: %d", planes[i].id);
        } else {
            LOG_WARNING("Failed to enqueue plane ID: %d (queue may be full)\n", planes[i].id);
        }
    }
    print_queue(&landing_queue);
    printf("\n-----------------------------------\n");
    // filter finished planes while preserving original order
    const int initial_size = landing_queue.size;
    for (int i = 0; i < initial_size; i++) {
        Airplane plane;
        if (dequeue(&landing_queue, &plane)) {
            if (!plane.finished) {
                enqueue(&landing_queue, plane);
            } else {
                LOG_INFO("Plane ID: %d is finished and removed from the queue.\n", plane.id);
            }
        }
    }

    print_queue(&landing_queue);
    return 0;
}