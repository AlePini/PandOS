cmake_minimum_required(VERSION "3.19.4")

project(PandOS)

include(GNUInstallDirs)

add_executable(${PROJECT_NAME} "p1test.c")

add_subdirectory(phase1)

target_link_libraries(${PROJECT_NAME} phase1)

target_link_directories(${PROJECT_NAME} PRIVATE phase1)

target_include_directories(${PROJECT_NAME}
                            PUBLIC
                            ${PROJECT_SOURCE_DIR}/h
                            ${PROJECT_SOURCE_DIR}/phase1)