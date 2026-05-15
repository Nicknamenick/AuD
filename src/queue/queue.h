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

int queue_size(const Queue *q);
void print_queue(const Queue *q);

#endif
