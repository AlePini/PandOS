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

/* This initializes Swap Pool Table and Swap Pool Semaphore
   it'll be called only by the initProc */
void initSwapStructs();

void pager();

#endif