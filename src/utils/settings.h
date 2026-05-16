#ifndef SIM_SETTINGS_H
#define SIM_SETTINGS_H

#define QUEUE_CAPACITY 100
#define VERBOSE 1 // 1: enable debug logs, 0: no logs

#define UPPER_FUEL_LIMIT 10
#define LOWER_FUEL_LIMIT 3

#define UPPER_PLANE_ARRIVAL_RATE 3 // max number of planes arriving at the same tick
#define LOWER_PLANE_ARRIVAL_RATE 2 // min number of planes arriving at the same tick

#define UPPER_PLANE_DEPARTURE_RATE 1 // max number of planes departing at the same tick
#define LOWER_PLANE_DEPARTURE_RATE 1 // min number of planes departing at the same tick
#endif
