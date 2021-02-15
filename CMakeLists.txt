cmake_minimum_required(VERSION "3.19.4")

project(PandOS)

include(GNUInstallDirs)

# Set UMPS3 Dirs
find_program(UMPS3 umps3)

# If you have umps3 installed on some weird location, set UMPS3_DIR_PREFIX by hand.
if(UMPS3)
    set(UMPS3_DIR_PREFIX /usr)
endif()

set(UMPS3_INCLUDE_DIR ${UMPS3_DIR_PREFIX}/include)
set(UMPS3_DATA_DIR ${UMPS3_DIR_PREFIX}/share/umps3)
set(UMPS3_LIB_DIR ${UMPS3_DIR_PREFIX}/lib/umps3)

# Compiler and linker setup
set(CMAKE_C_COMPILER mipsel-linux-gnu-gcc)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mips1 -mabi=32 -mno-gpopt -G 0 -mno-abicalls -fno-pic -mfp32 -I${UMPS3_INCLUDE_DIR} -Wall -O0")

set(CMAKE_LINKER mipsel-linux-gnu-ld)
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -G 0 -nostdlib -T ${UMPS3_DATA_DIR}/umpscore.ldscript")

add_executable(${PROJECT_NAME} p1test.c)

target_include_directories(${PROJECT_NAME}
                            PUBLIC
                            ${UMPS3_INCLUDE_DIR}
                            ${PROJECT_SOURCE_DIR}/h
                            ${PROJECT_SOURCE_DIR}/phase1)

add_subdirectory(phase1)

target_link_libraries(${PROJECT_NAME}
                        phase1
                        ${UMPS3_LIB_DIR}/crtso.o
                        ${UMPS3_LIB_DIR}/libumps.o)

target_link_directories(${PROJECT_NAME} PRIVATE phase1)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND umps3-elf2umps -k ${PROJECT_NAME})