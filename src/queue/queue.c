#include <stdio.h>
#include "queue.h"
#include "../airplane/airplane.h"
#include "../utils/logger.h"

void init_queue(Queue *q) {
    q->head = 0;
    q->tail = 0;
    q->size = 0;
}
/* Enqueues an airplane into the queue.
 * Returns true if successful, false if the queue is full. */
bool enqueue(Queue *q, const Airplane plane) {
    if (is_queue_full(q)) return false;
    q->data[q->tail] = plane;
    q->tail = (q->tail + 1) % QUEUE_CAPACITY;
    q->size++;
    return true;
}

/* Dequeues an airplane from the queue and stores it in the provided pointer.
 * Returns true if successful, false if the queue is empty. */
bool dequeue(Queue *q, Airplane *plane) {
    if (is_queue_empty(q)) return false;
    *plane = q->data[q->head];
    q->head = (q->head + 1) % QUEUE_CAPACITY;
    q->size--;
    return true;
}

bool is_queue_empty(const Queue *q) {
    return q->size == 0;
}

bool is_queue_full(const Queue *q) {
    return q->size == QUEUE_CAPACITY;
}

int queue_size(const Queue *q) {
    return q->size;
}

void print_queue(const Queue *q) {
    LOG_INFO("Queue (size: %d):", q->size);
    for (int i = 0; i < q->size; i++) {
        int index = (q->head + i) % QUEUE_CAPACITY;
        Airplane plane = q->data[index];
        LOG_INFO("  Plane ID: %d, Type: %s, Status: %s, Fuel: %d, finished: %s",
               plane.id,
               plane.type == PLANE_LANDING ? "Landing" : "Takeoff",
               plane.status == PLANE_WAITING ? "Waiting" :
               plane.status == PLANE_LANDED ? "Landed" :
               plane.status == PLANE_STARTED ? "Started" : "Crashed",
               plane.fuel,
                plane.finished ? "Yes" : "No"
               );
    }
}