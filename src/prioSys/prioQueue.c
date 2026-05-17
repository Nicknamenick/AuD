#include "../prioSys/prioQueue.h"
#include "../utils/logger.h"
#include "../utils/misc.h"

/*
 * This implementation uses a scoring system to decide which queue to place each plane in, based on:
 * - Fuel gap: how much the plane's fuel differs from the last plane in the queue
 * - Size balance: how much adding the plane would imbalance the queue sizes
 * - Risk of running out of fuel: how many planes would be ahead of this plane after adding it, compared to its fuel level
 * - Tie-breaking: if scores are equal, prefer the queue specified in the config
 * The weights for each factor can be configured in the PrioQueueConfig struct, allowing for different prioritization strategies.
 */

void init_prio_queue_config(PrioQueueConfig *config) {
    if (!config) {
        return;
    }
    config->fuel_gap_weight = 3;
    config->size_balance_weight = 60;
    config->tie_breaker_queue = 1;
    config->fuel_risk_weight = 70;
}

/*  */
static int fuel_gap_score(const Queue *q, const Airplane *buffer, const int buffer_size,
                          const Airplane *plane) {
    if (buffer_size > 0) {
        const int fuel_gap = abs_int(plane->fuel - buffer[buffer_size - 1].fuel);
        LOG_INFO("Fuel gap for plane ID %d with buffer: %d", plane->id, fuel_gap);
        return abs_int(fuel_gap);
    }
    if (!is_queue_empty(q)) {
        const Airplane last = peek_queue_last(q);
        const int fuel_gap = abs_int(plane->fuel - last.fuel);
        LOG_INFO("Fuel gap for plane ID %d with queue: %d", plane->id, fuel_gap);
        return abs_int(fuel_gap);
    }
    //LOG_WARNING("Fuel gap for plane ID %d with empty queue: 0", plane->id);
    return 0;
}
/* Calculates the risk of the plane running out of fuel */
static int queue_risk_value(const Queue *q, const int buffer_size, const Airplane *plane) {
    const int size_after = queue_size(q) + buffer_size + 1;
    const int risk = size_after - plane->fuel;
    return risk;
}
/* Calculates the score according to the config */
static int total_score_with_balance(const Queue *q, const Airplane *buffer, const int buffer_size,
                                    const Airplane *plane, const PrioQueueConfig *config,
                                    const int other_size) {
    const int fuel_gap = fuel_gap_score(q, buffer, buffer_size, plane);
    const int size_after = queue_size(q) + buffer_size + 1;
    const int risk = queue_risk_value(q, buffer_size, plane);
    const int imbalance = size_after > other_size ? (size_after - other_size) : 0;
    const int risk_penalty = risk > 0 ? risk : 0;
    if (risk_penalty > 5) {
        LOG_WARNING("Plane ID %d has high risk of running out of fuel: %d, adding risk score: %d", plane->id, risk_penalty, config->fuel_risk_weight * risk_penalty);
    }
    return (config->fuel_gap_weight * fuel_gap)
           + (config->size_balance_weight * imbalance)
           + (config->fuel_risk_weight * risk_penalty);
}

/* picks a queue according to the score */
static int pick_queue(const Airplane *plane,
                      const Queue *q1, const Queue *q2,
                      const Airplane *buffer1, const int buffer1_size,
                      const Airplane *buffer2, const int buffer2_size,
                      const PrioQueueConfig *config) {
    const int size1 = queue_size(q1) + buffer1_size;
    const int size2 = queue_size(q2) + buffer2_size;

    if (size1 >= QUEUE_CAPACITY && size2 >= QUEUE_CAPACITY) {
        return 0;
    }
    if (size1 >= QUEUE_CAPACITY) {
        return 2;
    }
    if (size2 >= QUEUE_CAPACITY) {
        return 1;
    }

    const int score1 = total_score_with_balance(q1, buffer1, buffer1_size, plane, config, size2);
    const int score2 = total_score_with_balance(q2, buffer2, buffer2_size, plane, config, size1);

    if (score1 < score2) {
        return 1;
    }
    if (score2 < score1) {
        return 2;
    }

    if (size1 < size2) {
        return 1;
    }
    if (size2 < size1) {
        return 2;
    }

    return config->tie_breaker_queue == 2 ? 2 : 1;
}

bool enqueue_landing_with_prio(const Airplane *planes, const int num_planes,
                               Queue *q1, Queue *q2, const PrioQueueConfig *config) {
    if (num_planes <= 0) {
        return true;
    }
    if (!planes || !q1 || !q2 || !config) {
        return false;
    }

    Airplane buffer1[QUEUE_CAPACITY];
    Airplane buffer2[QUEUE_CAPACITY];
    int buffer1_size = 0;
    int buffer2_size = 0;

    for (int i = 0; i < num_planes; i++) {
        const int choice = pick_queue(&planes[i], q1, q2, buffer1, buffer1_size, buffer2, buffer2_size, config);
        if (choice == 0) {
            return false;
        }
        if (choice == 1) {
            buffer1[buffer1_size++] = planes[i];
        } else {
            buffer2[buffer2_size++] = planes[i];
        }
    }

    if (buffer1_size > 0 && !enqueue_arr(q1, buffer1, buffer1_size)) {
        return false;
    }
    if (buffer2_size > 0 && !enqueue_arr(q2, buffer2, buffer2_size)) {
        return false;
    }

    return true;
}
