set(PHASE1 kernel-1)
set(PHASE2 kernel-2)
set(PHASE3 kernel-3)

# Phase 1
add_executable(${PHASE1} p1test.c)

set(LIB_PHASE1 phase1 ${UMPS3_LIB_DIR}/crtso.o ${UMPS3_LIB_DIR}/libumps.o)
set(INC_PHASE1 ${PROJECT_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/phase1)

target_include_directories(${PHASE1} PUBLIC ${INC_PHASE1})
target_link_libraries(${PHASE1} ${LIB_PHASE1})


# Phase 2
add_executable(${PHASE2} ${PROJECT_SOURCE_DIR}/phase2/initial.c)

set(LIB_PHASE2 ${LIB_PHASE1} phase2)
set(INC_PHASE2 ${INC_PHASE1} ${PROJECT_SOURCE_DIR}/phase2)

target_include_directories(${PHASE2} PUBLIC ${INC_PHASE2})
target_link_libraries(${PHASE2} ${LIB_PHASE2})


# Phase 3
add_executable(${PHASE3} ${PROJECT_SOURCE_DIR}/phase3/initProc.c)

set(LIB_PHASE3 ${LIB_PHASE2} phase3)
set(INC_PHASE3 ${INC_PHASE2} ${PROJECT_SOURCE_DIR}/phase3)

target_include_directories(${PHASE3} PUBLIC ${INC_PHASE3})
target_link_libraries(${PHASE3} ${LIB_PHASE3})


# Kernels
add_custom_command(TARGET ${PHASE1} POST_BUILD COMMAND umps3-elf2umps -k ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PHASE1})
add_custom_command(TARGET ${PHASE2} POST_BUILD COMMAND umps3-elf2umps -k ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PHASE2})
add_custom_command(TARGET ${PHASE3} POST_BUILD COMMAND umps3-elf2umps -k ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PHASE3})

# Devices
#set(DEVICES printerTest strConcat fibEight fibEleven 
#            terminalTest2 terminalTest3
#            terminalTest4 terminalTest5)

#set(LIB_PHASED ${UMPS3_LIB_DIR}/crtso.o ${UMPS3_LIB_DIR}/libumps.o)
#set(INC_PHASED ${PROJECT_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/testers)

#set(device printerTest)
#foreach(device IN LISTS DEVICES)
#    message(STATUS "${device}")
#    add_executable(printerTest printerTest.c h/tconst.h)
#    target_include_directories(printerTest PUBLIC ${INC_PHASED})
#endforeach()