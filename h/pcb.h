#ifndef PCB_H_INCLUDED
#define PCB_H_INCLUDED

#include <pandos_types.h>
#include <pandos_const.h>

/***************************************
*
* Definitions of Queue Manager functions
*
****************************************/

HIDDEN pcb_t pcbFree_table[MAXPROC];
HIDDEN pcb_t *pcbFree_h;

void freePcb(pcb_t *p);

pcb_t *allocPcb();

void initPcbs();

pcb_t *mkEmptyProcQ();

int emptyProcQ(pcb_t *tp);

void insertProcQ(pcb_t **tp, pcb_t *p);

pcb_t *removeProcQ(pcb_t **tp);

pcb_t *outProcQ(pcb_t **tp, pcb_t *p);

pcb_t *headProcQ(pcb_t *tp);

HIDDEN pcb_t* resetPcb(pcb_t* p);

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