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

/****************************************
*
* Definitions of Queue Manager functions
*
*****************************************/

HIDDEN pcb_t pcbFree_table[MAXPROC];
HIDDEN pcb_t *pcbFree_h;

/**
 * @brief Resets all the values of a pcb pointer to NULL.
 *
 * @param p The pointer to the PCB that has to be resetted.
 * @return The pointer to the pcb.
 */
HIDDEN pcb_t* resetPcb(pcb_t* p);

/**
 * @brief Inserts the element pointed by p onto the pcbFree list.
 *
 * @param p Pointer to the pcb that has to be put in the pcbFree list.
 */
void freePcb(pcb_t *p);

/**
 * @brief Removes an element from the pcbFree list and Initializes his values.
 *
 * @return NULL if the pcbFree list is empty otherwise a pointer to the removed pcb.
 */
pcb_t *allocPcb();

/**
 * @brief Initializes the pcbFree list. This function should be called only once during initialization.
 *
 */
void initPcbs();

/**
 * @brief Initializes a variable to be a tail pointer to a proces queue.
 *
 * @return A tail pointer to an empty process queue.
 */
pcb_t *mkEmptyProcQ();

/**
 * @brief Checks if the queue pointed by tp is empty.
 *
 * @param tp Tail pointer of the queue.
 * @return TRUE if the queue is empty, FALSE otherwise.
 */
int emptyProcQ(pcb_t *tp);

/**
 * @brief Inserts the pcb pointed by to by p into the queue pointed by tp.
 *
 * @param tp Tail pointer of the queue.
 * @param p Pointer to the pcb.
 */
void insertProcQ(pcb_t **tp, pcb_t *p);

/**
 * @brief Returns the pointer to the head of the tp process queue, without removing it.
 *
 * @param tp The pointer to the tail of the process queue.
 * @return The pointer to the head of the process queue, NULL if the queue is empty.
 */
pcb_t *headProcQ(pcb_t *tp);

/**
 * @brief Removes the oldest element (the head) from the tp queue.
 *
 * @param tp The pointer to the queue.
 * @return The pointer to the element removed from the list, NULL if the queue is empty.
 */
pcb_t *removeProcQ(pcb_t **tp);

/**
 * @brief Removes the PCB pointed by P from the process queue pointed by tp.
 *
 * @param tp The pointer to the queue.
 * @param p The pointer to the PCB that has to be removed.
 * @return The pointer to the removed PCB, NULL if the PCB pointed by p is not in the queue.
 */
pcb_t *outProcQ(pcb_t **tp, pcb_t *p);

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
HIDDEN pcb_t* trim(pcb_t *p);

/**
 * @brief Inspects if the PCB pointed by p has a child.
 *
 * @param p The pointer to the PCB that has to be inspected.
 * @return TRUE if the PCB pointed by p has no children, FALSE otherwise.
 */
int emptyChild(pcb_t *p);

/**
 * @brief Inserts the PCB pointed by p as a child of the PCB pointed by prnt.
 *
 * @param prnt The pointer to the PCB which will become parent of p.
 * @param p The pointer to the PCB which will become child of prnt.
 */
void insertChild(pcb_t *prnt, pcb_t *p);

/**
 * @brief Removes the first child of the PCB pointed by p.
 *
 * @param p The pointer to the PCB whose first child will be removed.
 * @return The pointer to the first child of the PCB, NULL if the PCB doesn't have a child.
 */
pcb_t *removeChild(pcb_t *p);

/**
 * @brief Removes the PCB pointed by p from the list of his parent's children.
 *
 * @param p The pointer to the PCB that will be removed.
 * @return The pointer to the PCB, NULL if the PCB doesn't have a parent.
 */
pcb_t *outChild(pcb_t *p);

#endif