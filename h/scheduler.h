#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <pandos_const.h>
#include <pandos_types.h>
#include <umps3/umps/cp0.h>

//Variabili
extern int processCount;
extern int softblockCount;
extern pcb_t* readyQueue;
extern pcb_t* currentProcess;


void scheduler();

#endif