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

void vm_break();

void initSwapStructs();

void clearSwap(int asid);

int replacementAlgorithm();

void updateTLB(pteEntry_t *newEntry);

void executeFlashAction(int deviceNumber, unsigned int pageIndex, unsigned int command, support_t *support);

void readFlash(int deviceNumber, unsigned int blockIndex, unsigned int pageIndex, support_t *support);

void writeFlash(int deviceNumber, unsigned int blockIndex, unsigned int pageIndex, support_t *support);

void pager();

void uTLB_RefillHandler();


#endif