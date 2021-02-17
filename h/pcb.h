#ifndef PCB_H_INCLUDED
#define PCB_H_INCLUDED

#include "pandos_types.h"
#include "pandos_const.h"

/***************************************
*
* Definitions of Queue Manager actions
*
****************************************/

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

pcb_t* resetPcb(pcb_t* p);

void head_insert(pcb_t* blist,pcb_t* elem);

void tail_insert(pcb_t* blist, pcb_t* elem);

void printList(pcb_t* lhead, int limit);

void reversePrintList(pcb_t* lhead, int limit);

/*****************************
*
* Definitions of Process Tree
*
******************************/

int emptyChild(pcb_t *p);

void insertChild(pcb_t *prnt, pcb_t *p);

pcb_t *removeChild(pcb_t *p);

pcb_t *outChild(pcb_t *p);

pcb_t* trim(pcb_t *p);

#endif