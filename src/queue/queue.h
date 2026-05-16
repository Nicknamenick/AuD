#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include "../utils/settings.h"
#include "../airplane/airplane.h"

typedef struct queue {
    Airplane data[QUEUE_CAPACITY];
    int head;
    int tail;
    int size;
} Queue;

void init_queue(Queue *q);

bool is_queue_empty(const Queue *q);
bool is_queue_full(const Queue *q);

bool enqueue(Queue *q, Airplane plane);
bool dequeue(Queue *q, Airplane *plane);

bool enqueue_arr(Queue *queue, const Airplane *planes, int num_planes);

Airplane peek_queue_first(const Queue *q);
Airplane peek_queue_last(const Queue *q);

int queue_size(const Queue *q);
void print_queue(const Queue *q);

extern Queue landing_queue_1;
extern Queue landing_queue_2;
extern Queue takeoff_queue_1;
extern Queue emergency_queue;

#endif
