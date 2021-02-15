#ifndef SEMAPHORE_H_INCLUDED
#define SEMAPHORE_H_INCLUDED

/****************************************************************************
 *
 * This header file contains definition of Active Semaphore List.
 *
 ***************************************************************************/
#include "pandos_const.h"
#include "pandos_types.h"
#include "pcb.h"

HIDDEN semd_t semd_table[MAXSEM];
HIDDEN semd_t* semdFree_h;
HIDDEN semd_t* semd_h;

int insertBlocked(int *semAdd,pcb_t *p);

pcb_t* removeBlocked(int *semAdd);

pcb_t* outBlocked(pcb_t *p);

pcb_t* headBlocked(int *semAdd);

void initASL();

#endif