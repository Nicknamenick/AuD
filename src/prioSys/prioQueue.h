#ifndef PRIO_QUEUE_H
#define PRIO_QUEUE_H

#include <stdbool.h>
#include "../airplane/airplane.h"
#include "../queue/queue.h"

typedef struct {
    int fuel_gap_weight;
    int size_balance_weight;
    int tie_breaker_queue;
    int fuel_risk_weight;
} PrioQueueConfig;

void init_prio_queue_config(PrioQueueConfig *config);

bool enqueue_landing_with_prio(const Airplane *planes, int num_planes, Queue *q1, Queue *q2,
                               const PrioQueueConfig *config);

#endif
