#ifndef SEMAPHORE_H_INCLUDED
#define SEMAPHORE_H_INCLUDED

/**
 * @file    ASL
 * @author  Juri Fabbri, Alessandro Filippini, Filippo Bertozzi, Leonardo Giacomini
 * @brief   Active Semaphore List methods.
 * @version 0.2
 * @date    2021-02-18
 */

#include <pandos_const.h>
#include <pandos_types.h>

/*************************************************
 *
 * Definition of Active Semaphore List functions.
 *
 *************************************************/

/**
 * @brief Insert the pcb pointed to by p at the tail of the process queue associated
with the semaphore whose physical address is semAdd and
set the semaphore address of p to semAdd.
 *
 * @param semAdd Semaphore identifier.
 * @param p Pointer to the PCB to be inserted.
 * @return TRUE if a new semaphore descriptor needs to be allocated, FALSE otherwise.
 * @remarks TRUE and FALSE are declarated in pandos_const.h and are respectively 1 and 0.
 */
int insertBlocked(int *semAdd,pcb_t *p);

/**
 * @brief Search for a semaphore whose descriptor is semADD. Remove the first pcb from
its process queue and return apointer to it.
 *
 * @param semAdd Semaphore identifier.
 * @return the pointer to the head from the process queue associated with the semaphore descriptor.
 */
pcb_t* removeBlocked(int *semAdd);

/**
 * @brief Remove the pcb pointed to by p from the process queue associated
 * with p’s semaphore.
 * @param p  Pointer to the pcb to be removed.
 * @return a pointer to the removed PCB. Returns NULL if p does not appear in the process queue.
 */
pcb_t* outBlocked(pcb_t *p);


/**
 * @brief The a pointer to the head of the process queue associated with the semaphore semAdd
 * @param semAdd Semaphore identifier.
 * @return the first element of the process queue associated with the semaphore semAdd.
 * Returns NULL if semAdd is not found.
 */
pcb_t* headBlocked(int *semAdd);

/**
 * @brief Initialize the semdFree list, this method will be only called once during data structure initialization.
 */
void initASL();

#endif