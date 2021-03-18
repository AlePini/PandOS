#ifndef SCHEDULER_H
#define SCHEDULER_H

extern int processCount;
extern int softblockCount;
extern pcb_t* readyQueue;
extern pcb_t* currentProcess;

#endif