#ifndef SIM_AIRPLANE_H
#define SIM_AIRPLANE_H
#include <stdbool.h>

typedef enum {
    PLANE_LANDING,
    PLANE_TAKEOFF
} PlaneType;

typedef enum {
    PLANE_WAITING,
    PLANE_LANDED,
    PLANE_STARTED,
    PLANE_CRASHED
} PlaneStatus;

typedef struct airplane {
    int id;
    PlaneType type;
    PlaneStatus status;
    int fuel;
    bool finished;
    int time_in_queue;
    int runway;
    int time_processed;
} Airplane;

extern int get_new_plane_id();

#endif
