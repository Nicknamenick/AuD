#include "../prioSys/prioEarly.h"
#include "../airplane/airplane.h"
/*
 * Basic fuel-based prioritization using selection sort.
 * Planes with less fuel will be placed earlier in the array.
 */
Airplane* ssort_fuel_basic(Airplane *planes, const int num_planes) {
    for (int i = 0; i < num_planes - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < num_planes; j++) {
            if (planes[j].fuel < planes[min_idx].fuel) {
                min_idx = j;
            }
        }
        const Airplane temp = planes[i];
        planes[i] = planes[min_idx];
        planes[min_idx] = temp;
    }
    return planes;
}
