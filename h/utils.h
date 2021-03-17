#ifndef UTILS_H
#define UTILS_H

#include <pandos_const.h>
#include <pandos_types.h>

pcb_t* getReadyQueue(){
    return &ready_queue;
}

#endif