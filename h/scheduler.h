#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <pandos_const.h>
#include <pandos_types.h>

extern int processCount;
extern int softblockCount;
extern pcb_t* readyQueue;
extern pcb_t* currentProcess;

#endif