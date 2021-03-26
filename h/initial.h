#ifndef INITIAL_H
#define INITIAL_H
#include <pandos_const.h>
#include <pandos_types.h>

//Dichiarazione variabili
int processCount;
int softblockCount;
pcb_t* readyQueue;
pcb_t* currentProcess;

SEMAPHORE semaphoreList[DEVICE_NUMBER];
SEMAPHORE semIntTimer;

#endif