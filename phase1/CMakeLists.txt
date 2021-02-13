add_library(phase1 
            ${PROJECT_SOURCE_DIR}/h/asl.h
            ${PROJECT_SOURCE_DIR}/h/queue.h
            ${PROJECT_SOURCE_DIR}/h/tree.h
            asl.c
            queue.c
            tree.c)

target_include_directories(phase1 PUBLIC ${PROJECT_SOURCE_DIR}/h)