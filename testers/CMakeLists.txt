add_executable(${PROJECT_NAME} p1test.c)

target_include_directories(${PROJECT_NAME}
                            PUBLIC
                            ${UMPS3_INCLUDE_DIR}
                            ${PROJECT_SOURCE_DIR}/h
                            ${PROJECT_SOURCE_DIR}/phase1)

target_link_libraries(${PROJECT_NAME}
                        phase1
                        ${UMPS3_LIB_DIR}/crtso.o
                        ${UMPS3_LIB_DIR}/libumps.o)

target_link_directories(${PROJECT_NAME} PRIVATE phase1)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND umps3-elf2umps -k ${PROJECT_NAME})