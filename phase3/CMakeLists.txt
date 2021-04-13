include_directories(phase3
                    ${PROJECT_INCLUDE_DIRS})

add_library(phase3
            initProc.c
            sysSupport.c
            vmSupport.c)