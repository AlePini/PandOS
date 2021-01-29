#ifndef PANDOS_PCBQ_H_INCLUDED
#define PANDOS_PCBQ_H_INCLUDED

/****************************************************************************
 *
 * This header file contains definitions of Queue Manager actions.
 *
 ****************************************************************************/

#include "pandos_types.h"

static pcb_t pcbFree_table[MAXPROC];
pcbFree *pcbFree_h;

void freePcb(pcb_t *p);

pcb_t *allocPcb();

void initPcbs();

pcb_t *mkEmptyProcQ();

int emptyProcQ(pcb_t *tp);

void insertProcQ(pcb_t **tp, pcb_t *p);

pcb_t *removeProcQ(pcb_t *tp);

pcb_t *outProcQ(pcb_t **tp, pcb_t *p);

pcb_t *headProcQ(pcb_t *tp);

#endif