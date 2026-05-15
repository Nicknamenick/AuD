#include "airplane.h"

int get_new_plane_id() {
    static int id_counter = 1;
    return id_counter++;
}