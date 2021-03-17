#ifndef INITIAL_H
#define INITIAL_H


#include <pandos_const.h>
#include <pandos_types.h>
#include <pcb.h>
#include <asl.h>
#include <p2test.c> //TODO: serve davvero? provare a compilare senza

extern void test();
extern void uTLB_RefillHandler();

HIDDEN int processCount;
HIDDEN int softblockCount;
HIDDEN pcb_t* readyQueue;
HIDDEN pcb_t* currentProcess;
HIDDEN int dev_sem[DEVICENUMBER];

#endif
