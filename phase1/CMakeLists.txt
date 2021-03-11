add_library(phase1
            asl.c
            pcb.c)

target_include_directories(phase1
                            PUBLIC
                            ${UMPS3_INCLUDE_DIR}
                            ${PROJECT_SOURCE_DIR}/h)