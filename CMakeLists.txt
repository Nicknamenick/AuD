cmake_minimum_required(VERSION 3.28)
project(sim C)

set(CMAKE_C_STANDARD 11)

add_executable(sim src/main.c
        src/utils/logger.c
        src/utils/logger.h
        src/queue/queue.c
        src/queue/queue.h
        src/prioSys/prioQueue.c
        src/prioSys/prioQueue.h
        src/airplane/airplane.c
        src/airplane/airplane.h
        src/utils/settings.h
        src/utils/misc.c
        src/utils/misc.h
        src/manager/manager.c
        src/manager/manager.h
        src/prioSys/prioEarly.c
        src/prioSys/prioEarly.h
        src/prioSys/prioQueue.c
)
