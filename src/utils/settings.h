#ifndef SIM_SETTINGS_H
#define SIM_SETTINGS_H

#define QUEUE_CAPACITY 100
#define VERBOSE 1 // 1: enable debug logs, 0: no logs

#define UPPER_FUEL_LIMIT 15
#define LOWER_FUEL_LIMIT 0
#define EMERGENCY_FUEL_THRESHOLD 2

#define UPPER_PLANE_ARRIVAL_RATE 3 // max number of planes arriving at the same tick
#define LOWER_PLANE_ARRIVAL_RATE 3 // min number of planes arriving at the same tick

#define UPPER_PLANE_DEPARTURE_RATE 1 // max number of planes departing at the same tick
#define LOWER_PLANE_DEPARTURE_RATE 0 // min number of planes departing at the same tick

#define CRASH_THRESHOLD 0 // planes with fuel <= this value will crash if not processed immediately
#define MAX_CRASHES_PER_TICK (QUEUE_CAPACITY * 3) // crashes can only come from landing_queue_1, landing_queue_2 and emergency_queue, so max crashes per tick is QUEUE_CAPACITY * 3
#endif
