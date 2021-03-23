include_directories(phase2
                    ${PROJECT_INCLUDE_DIRS})

add_library(phase2
            exceptions.c
            interrupts.c
            memory.c
            scheduler.c
            utils.c
            ${PROJECT_SOURCE_DIR}/testers/p2test.c)