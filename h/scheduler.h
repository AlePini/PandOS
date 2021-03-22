#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <pandos_const.h>
#include <pandos_types.h>

//Variabili
extern int processCount;
extern int softblockCount;
extern pcb_t* readyQueue;
extern pcb_t* currentProcess;

//Funzioni
extern pcb_t* getReadyQueue();

void scheduler();

#endif