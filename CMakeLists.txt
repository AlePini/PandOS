cmake_minimum_required(VERSION "3.13.0")

project(PandOS VERSION 0.1)

set(CMAKE_INSTALL_PREFIX /usr)
include(GNUInstallDirs)

# Set UMPS3 Dirs
find_program(UMPS3 umps3)

if(NOT UMPS3)
    message(FATAL_ERROR "UMPS3 not found.")
endif()

# If you have umps3 installed on some weird location,
# set UMPS3_DIR_PREFIX by hand. or set them individually.
if(NOT UMPS3_DIR_PREFIX)
    set(UMPS3_DIR_PREFIX ${CMAKE_INSTALL_PREFIX})
endif()

if(NOT UMPS3_INCLUDE_DIR)
    set(UMPS3_INCLUDE_DIR ${CMAKE_INSTALL_FULL_INCLUDEDIR})
endif()

if(NOT UMPS3_DATA_DIR)
    set(UMPS3_DATA_DIR ${CMAKE_INSTALL_FULL_DATADIR}/umps3)
endif()

if(NOT UMPS3_LIB_DIR)
    set(UMPS3_LIB_DIR ${CMAKE_INSTALL_FULL_LIBDIR}/umps3)
endif()

# Compiler and linker setup
set(CMAKE_C_COMPILER mipsel-linux-gnu-gcc)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mips1 -mabi=32 -mno-gpopt -G 0 -mno-abicalls -fno-pic -mfp32 -I${UMPS3_INCLUDE_DIR} -Wall -O0")

set(CMAKE_LINKER mipsel-linux-gnu-ld)
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -G 0 -nostdlib -T ${UMPS3_DATA_DIR}/umpscore.ldscript")

set(PROJECT_INCLUDE_DIRS ${UMPS3_INCLUDE_DIR} ${PROJECT_SOURCE_DIR}/h)

add_subdirectory(phase1)
add_subdirectory(phase2)

add_subdirectory(testers)
