set(PHASE1 kernel-1)
set(PHASE2 kernel-2)

# Phase 1
add_executable(${PHASE1} p1test.c)

target_include_directories(${PHASE1}
                            PUBLIC
                            ${UMPS3_INCLUDE_DIR}
                            ${PROJECT_SOURCE_DIR}/h
                            ${PROJECT_SOURCE_DIR}/phase1)

target_link_libraries(${PHASE1}
                        phase1
                        ${UMPS3_LIB_DIR}/crtso.o
                        ${UMPS3_LIB_DIR}/libumps.o)

target_link_directories(${PHASE1} PRIVATE phase1)

# Phase 2
add_executable(${PHASE2} p2test.c)

target_include_directories(${PHASE2}
                            PUBLIC
                            ${UMPS3_INCLUDE_DIR}
                            ${PROJECT_SOURCE_DIR}/h
                            ${PROJECT_SOURCE_DIR}/phase1
                            ${PROJECT_SOURCE_DIR}/phase2
                            )

target_link_libraries(${PHASE2}
                        phase1
                        phase2
                        ${UMPS3_LIB_DIR}/crtso.o
                        ${UMPS3_LIB_DIR}/libumps.o)

target_link_directories(${PHASE2} PRIVATE phase1 phase2)

# Kernels
add_custom_command(TARGET ${PHASE1} POST_BUILD COMMAND umps3-elf2umps -k ${PHASE1})
add_custom_command(TARGET ${PHASE2} POST_BUILD COMMAND umps3-elf2umps -k ${PHASE2})