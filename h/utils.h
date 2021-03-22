#ifndef UTILS_H
#define UTILS_H

#include <pandos_types.h>

//Variabili
extern pcb_t* readyQueue;

//Funzioni
extern int processCount;
extern int softblockCount;
extern pcb_t* currentProcess;

pcb_t* getReadyQueue();

#endif