/****************************************************************************
 *
 * DELTE THIS COMMENT WHEN YOU'RE DONE
 *
 * This module implements : 
 * [ ] TLB Exception handler
 * [x] local Swap Pool Semaphore mutual exclusion sem
 * [x] local Swap Pool Table - implemented in pandos_types (swap_t struct)
 * 
 ****************************************************************************/

#ifndef VMSUPPORT_H
#define VMSUPPORT_H

#include <pandos_types.h>

/* This initializes Swap Pool Table and Swap Pool Semaphore
   it'll be called only by the initProc */
void initSwapStructs();

int replacementAlgorithm();

void updateTLB(pteEntry_t *newEntry);

bool checkMutexBusy();

void PSV();

bool checkOccupied(int i);

void executeFlashAction(int deviceNumber, unsigned int pageIndex, unsigned int action, support_t *support);

void readFlash(int deviceNumber, unsigned int blockIndex, unsigned int pageIndex, support_t *support);

void writeFlash(int deviceNumber, unsigned int blockIndex, unsigned int pageIndex, support_t *support);

void pager();

void uTLB_RefillHandler();

#endif