add_library(phase2
            exceptions.c
            initial.c
            interrupts.c
            scheduler.c)

include_directories(phase2
                    ${PROJECT_INCLUDE_DIRS})