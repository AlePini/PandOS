#ifndef SEMAPHORE_H_INCLUDED
#define SEMAPHORE_H_INCLUDED

/****************************************************************************
 *
 * This header file contains definition of Active Semaphore List.
 *
 ***************************************************************************/
#include "pandos_const.h"
#include "pandos_types.h"
#include <pcb.h>

HIDDEN semd_t semd_table[MAXSEM];
HIDDEN semd_t* semdFree_h;
HIDDEN semd_t* semd_h;

/**
 * @brief   This function takes as input a semAdd and returns the semaphore in semd_h with the
 * identifier of maximum value but lower than the one passed as argument.
 * @param semAdd    Physical address of the process queue
 * @return the previous semd_t of the one identified by semAdd
 */
HIDDEN semd_t* findPrevSem(int* semAdd);

int insertBlocked(int *semAdd,pcb_t *p);

pcb_t* removeBlocked(int *semAdd);

pcb_t* outBlocked(pcb_t *p);

pcb_t* headBlocked(int *semAdd);

void initASL();

#endif