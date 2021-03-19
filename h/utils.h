#ifndef UTILS_H
#define UTILS_H

#include <pandos_types.h>

extern pcb_t* readyQueue;
extern int processCount;
extern int softblockCount;
extern pcb_t* currentProcess;

pcb_t* getReadyQueue();

#endif