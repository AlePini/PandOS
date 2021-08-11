include_directories(phase2
                    ${PROJECT_INCLUDE_DIRS})

add_library(phase2
            exceptions.c
            interrupts.c
            memory.c
            scheduler.c
            syscalls.c
            ${PROJECT_SOURCE_DIR}/testers/p2test.c)

# Phase2 library used in Phase3
include_directories(phase2b
            ${PROJECT_INCLUDE_DIRS})

add_library(phase2b
            exceptions.c
            initial.c
            interrupts.c
            memory.c
            scheduler.c
            syscalls.c)

target_link_libraries(phase2b phase1)