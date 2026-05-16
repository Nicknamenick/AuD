#include <stdio.h>
#include "queue.h"
#include "../airplane/airplane.h"
#include "../utils/logger.h"

Queue landing_queue_1 = { .head = 0, .tail = 0, .size = 0 };
Queue landing_queue_2 = { .head = 0, .tail = 0, .size = 0 };
Queue takeoff_queue_1 = { .head = 0, .tail = 0, .size = 0 };
Queue emergency_queue = { .head = 0, .tail = 0, .size = 0 };

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

bool enqueue_arr(Queue *queue, const Airplane *planes, const int num_planes) {
    for (int i = 0; i < num_planes; i++) {
        if (!enqueue(queue, planes[i])) {
            LOG_ERROR("Failed to enqueue plane ID %d into queue: %p", planes[i].id, (void*)queue);
            return false;
        } else {
            LOG_INFO("Enqueued plane ID %d into queue %p", planes[i].id, (void*)queue);
        }
    }
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

/* Prints the contents of the queue for debugging purposes. */
void print_queue(const Queue *q) {
    LOG_INFO("Queue (size: %d):", q->size);
    for (int i = 0; i < q->size; i++) {
        int index = (q->head + i) % QUEUE_CAPACITY;
        Airplane plane = q->data[index];
        LOG_INFO("  Plane ID: %d, Type: %s, Status: %s, Fuel: %d",
               plane.id,
               plane.type == PLANE_LANDING ? "Landing" : "Takeoff",
               plane.status == PLANE_WAITING ? "Waiting" :
               plane.status == PLANE_QUEUED ? "Queued" :
               plane.status == PLANE_FINISHED ? "Finished" : "Crashed",
               plane.fuel);
    }
}

/* get the first element of a queue */
Airplane peek_queue_first(const Queue *q) {
    if (is_queue_empty(q)) {
        LOG_WARNING("Attempted to peek first element of an empty queue.");
        return (Airplane){0};
    }
    return q->data[q->head];
}

/* get the last element of a queue */
Airplane peek_queue_last(const Queue *q) {
    if (is_queue_empty(q)) {
        LOG_WARNING("Attempted to peek last element of an empty queue.");
        return (Airplane){0};
    }
    const int last_index = (q->tail - 1 + QUEUE_CAPACITY) % QUEUE_CAPACITY;
    return q->data[last_index];
}