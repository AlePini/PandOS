#ifndef INITIAL_H
#define INITIAL_H
#include <pandos_const.h>
#include <pandos_types.h>

//Dichiarazione variabili
extern int processCount;
extern int softblockCount;
extern pcb_t* readyQueue;
extern pcb_t* currentProcess;

extern SEMAPHORE semaphoreList[];
extern SEMAPHORE swiSemaphore;

#endif