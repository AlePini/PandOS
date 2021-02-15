#ifndef QUEUES_H_INCLUDED
#define QUEUES_H_INCLUDED

/****************************************************************************
 *
 * This header file contains definitions of Queue Manager actions.
 *
 ***************************************************************************/

#include "pandos_types.h"
#include "pandos_const.h"

HIDDEN pcb_t pcbFree_table[MAXPROC];
HIDDEN pcb_t *pcbFree_h;

// FUNZIONI DA IMPLEMENTARE PER PANDOS

void freePcb(pcb_t *p);

pcb_t *allocPcb();

void initPcbs();

pcb_t *mkEmptyProcQ();

int emptyProcQ(pcb_t *tp);

void insertProcQ(pcb_t **tp, pcb_t *p);

pcb_t *removeProcQ(pcb_t **tp);

pcb_t *outProcQ(pcb_t **tp, pcb_t *p);

pcb_t *headProcQ(pcb_t *tp);

// FUNZIONI AUSILIARIE PER IMPLEMENTARE LE ALTRE

void head_insert(pcb_t* blist,pcb_t* elem);

void tail_insert(pcb_t* blist, pcb_t* elem);

void printList(pcb_t* lhead, int limit);

void reversePrintList(pcb_t* lhead, int limit);

#endif