#ifndef SIM_AIRPLANE_H
#define SIM_AIRPLANE_H

typedef enum {
    PLANE_LANDING,
    PLANE_TAKEOFF
} PlaneType;

typedef enum {
    PLANE_WAITING,
    PLANE_QUEUED,
    PLANE_FINISHED,
    PLANE_CRASHED
} PlaneStatus;

typedef struct airplane {
    int id;
    PlaneType type;
    PlaneStatus status;
    int fuel;
    int time_in_queue;
    int runway;
    int time_processed;
} Airplane;

extern int get_new_plane_id();

int create_new_plane_set_arriving(Airplane *plane_set);
int create_new_plane_set_departing(Airplane *plane_set);

void print_airplane(const Airplane *plane);

#endif

/*
+-----------------------+         +-----------------------+
|       <<enum>>        |         |       <<enum>>        |
|       PlaneType       |         |      PlaneStatus      |
+-----------------------+         +-----------------------+
| + PLANE_LANDING       |         | + PLANE_WAITING       |
| + PLANE_TAKEOFF       |         | + PLANE_QUEUED        |
+-----------------------+         | + PLANE_FINISHED      |
             ^                    | + PLANE_CRASHED       |
             |                    +-----------------------+
             |                                ^
             |                                |
             +---------------+----------------+
                             |
+---------------------------------------------------------+
|                       <<struct>>                        |
|                        Airplane                         |
+---------------------------------------------------------+
| + id: int                                               |
| + type: PlaneType                                       |
| + status: PlaneStatus                                   |
| + fuel: int                                             |
| + time_in_queue: int                                    |
| + runway: int                                           |
| + time_processed: int                                   |
+---------------------------------------------------------+
 *
 */