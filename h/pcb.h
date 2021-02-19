#ifndef PCB_H_INCLUDED
#define PCB_H_INCLUDED

/**
 * @file    PCB
 * @author  Juri Fabbri, Alessandro Filippini, Filippo Bertozzi, Leonardo Giacomini
 * @brief   Process Control Block methods.
 * @version 0.1
 * @date 2021-02-18
 *
 */

#include <pandos_types.h>
#include <pandos_const.h>

/***************************************
*
* Definitions of Queue Manager functions
*
****************************************/

HIDDEN pcb_t pcbFree_table[MAXPROC];
HIDDEN pcb_t *pcbFree_h;

HIDDEN pcb_t* resetPcb(pcb_t* p);

void freePcb(pcb_t *p);

pcb_t *allocPcb();

void initPcbs();

pcb_t *mkEmptyProcQ();

int emptyProcQ(pcb_t *tp);

void insertProcQ(pcb_t **tp, pcb_t *p);

pcb_t *removeProcQ(pcb_t **tp);

pcb_t *outProcQ(pcb_t **tp, pcb_t *p);

pcb_t *headProcQ(pcb_t *tp);



/****************************************
*
* Definitions of Process Tree functions
*
****************************************/

/**
 * @brief This funcion takes as input a pointer to a PCB who has
 * to be removed from his tree.
 * @param p The pcb pointer that has to be removed from his tree
 * @return the pointer to the PCB whose fields have been set to NULL
 */
HIDDEN pcb_t* trim(pcb_t *p)

int emptyChild(pcb_t *p);

void insertChild(pcb_t *prnt, pcb_t *p);

pcb_t *removeChild(pcb_t *p);

pcb_t *outChild(pcb_t *p);

pcb_t* trim(pcb_t *p);

#endif