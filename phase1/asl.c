// #include <asl.h>

// HIDDEN semd_t semd_table[MAXSEM];
// HIDDEN semd_t* semdFree_h;
// HIDDEN semd_t* semd_h;

// /**This function works also when the semaphore we're searching for is not present in the ASL.
// *The function will return the last semaphore before the one whose identifier is MAXINT and
// *the checks in the other functions will handle error situations.
// */

// HIDDEN semd_t* findPrevSem(int* semAdd){
//     //Scan the ASL until the next semaphore identifier is bigger than semAdd
//     semd_t* head = semd_h;
//     while(head->s_next->s_semAdd < semAdd){
//         head = head->s_next;
//     }
//     return head;
// }

// int insertBlocked(int *semAdd, pcb_t *p){
//     //Checks if the PCB pointer passed is not NULL
//     if (p == NULL) return FALSE;

//     semd_t* prev = findPrevSem(semAdd);
//     semd_t* next = prev->s_next;
//     //If the semaphore with the correct identifier is found the function inserts the process p.
//     if(next->s_semAdd == semAdd){
//         p->p_semAdd  = semAdd;
//         insertProcQ(&next->s_procQ, p);
//     }
//     //Otherwise it has to allocate a new semaphore with the right identifier.
//     else{

//         //If the list of the free semaphores is empty returns true.
//         if (semdFree_h == NULL) return TRUE;

//         //Otherwise allocates and initializes one.
//         semd_t* toInsert = semdFree_h;
//         semdFree_h = semdFree_h -> s_next;
//         toInsert -> s_semAdd = semAdd;
//         toInsert -> s_procQ = mkEmptyProcQ();
//         p->p_semAdd  = semAdd;
//         insertProcQ(&toInsert->s_procQ, p);

//         //Then inserts it in the list of the actives semaphores.
//         toInsert -> s_next = prev-> s_next;
//         prev -> s_next = toInsert;
//     }
//     return FALSE;
// }

// pcb_t* removeBlocked(int *semAdd){

//     semd_t* prev = findPrevSem(semAdd);
//     semd_t* next = prev->s_next;

//     //If the semaphore the function is looking for is not there returns NULL.
//     if(next->s_semAdd != semAdd) return NULL;

//     //Otherwise removes the head of the queue of the processes.
//     pcb_t* removed = removeProcQ(&next->s_procQ);
//     //If by doing so the process queue becomes empty, deallocates the semaphore.
//     if(emptyProcQ(next->s_procQ)){
//         //Returns the empty semaphore to the semdFree.
//         prev->s_next=next->s_next;
//         next->s_semAdd = NULL;
//         next->s_next=semdFree_h;
//         semdFree_h=next;
//     }
//     return removed;
// }

// pcb_t* outBlocked(pcb_t *p) {
//     //Checks if the PCB pointer passed is not NULL
//     if (p == NULL) return NULL;
//     semd_t* prev = findPrevSem(p->p_semAdd);
//     semd_t* next = prev->s_next;

//     //If the semaphore is not in the ASL returns NULL.
//     if(next->s_semAdd != p->p_semAdd) return NULL;
//     //Otherwise removes p from the process queue.
//     pcb_t *toRemove = outProcQ(&(next->s_procQ), p);
//     //If by doing so the process queue becomes empty, deallocates the semaphores.
//     if(emptyProcQ(next->s_procQ)){
//         //Returns the empty semaphores to the semdFree.
//         prev->s_next=next->s_next;
//         next->s_semAdd = NULL;
//         next->s_next=semdFree_h;
//         semdFree_h=next;
//     }
//     return toRemove;
// }

// pcb_t* headBlocked(int *semAdd){
//     semd_t* prev = findPrevSem(semAdd);
//     semd_t* next = prev->s_next;
//     //If the semaphores is not in the ASL returns NULL.
//     if(next->s_semAdd != semAdd)
//         return NULL;
//     //Otherwise returns the head of the process queue.
//     return headProcQ(next->s_procQ);
// }


// void initASL(){

//     //Adds the two extra sempaphores of the actives ones' list.
//     //The one with the identifier = MININT which is the first of the table.
//     semd_h = &semd_table[0];
//     semd_h -> s_semAdd = MININT;
//     //The one with the identifier = MAXINT which is the second of the table.
//     semd_h ->s_next = &semd_table[1];
//     semd_h ->s_next->s_semAdd = MAXINT;
//     //Ends with NULL.
//     semd_h->s_next->s_next = NULL;

//     //Associo alla lista dei semafori liberi al primo disponibile nella table
//     semdFree_h = &semd_table[2];
//     semd_t* tmp = semdFree_h;
//     //Adds to the semdFree list all the other semaphores until their number is equal to MAXPROC.
//     for(int i=3; i<MAXSEM; i++){
//         tmp->s_next = &semd_table[i];
//         tmp = tmp->s_next;
//     }
//     //Ends with NULL.
//     tmp->s_next = NULL;
// }

#include "asl.h"
#include "pcb.h"

#define MAXSEMD (MAXPROC + 2)

/**
 * @brief A static array containing all
 * possible semaphore descriptors.
 * 
 */
HIDDEN semd_t semd_table[MAXSEMD];

/**
 * @brief List containing all free
 * semaphore descriptors.
 * 
 * @remark Contains two dummy nodes
 * for better management.
 * 
 */
HIDDEN semd_t *semdFree_h;

/**
 * @brief List containing all active
 * semaphore descriptors.
 * 
 */
HIDDEN semd_t *semd_h;

HIDDEN semd_t* findPrevSemd(int *semAdd) {
    // By returning the previous element instead
    // of the element itself, it is possible
    // to perform more operations on it (such as
    // removing it from the list)
    semd_t *prev = semd_h;
    while (semAdd > prev->s_next->s_semAdd) {
        prev = prev->s_next;
    }

    return prev;
}

int insertBlocked(int *semAdd, pcb_t *p) {
    semd_t *prev = findPrevSemd(semAdd);
    semd_t *element = prev->s_next;

    // If element->s_semAdd == semAdd, the
    // element was found
    if (element->s_semAdd == semAdd) {
        p->p_semAdd = semAdd;
        insertProcQ(&(element->s_procQ), p);
    }
    else {
        if (semdFree_h == NULL){
            // The free list is empty, cannot allocate
            // more semaphore descriptors
            return TRUE;
        }

        // Allocate a new semaphore descriptor
        semd_t *newSemd = semdFree_h;
        semdFree_h = semdFree_h->s_next;

        prev->s_next = newSemd;
        newSemd->s_next = element;
        newSemd->s_semAdd = semAdd;
        newSemd->s_procQ = mkEmptyProcQ();
        p->p_semAdd = semAdd;

        // Insert the p into the semaphore
        // descriptor's process list
        insertProcQ(&(newSemd->s_procQ), p);
    }

    return FALSE;
}

pcb_t* removeBlocked(int *semAdd) {
    semd_t *prev = findPrevSemd(semAdd);
    semd_t *element = prev->s_next;

    // If element->s_semAdd == semAdd, the
    // element was found
    if (element->s_semAdd == semAdd){
        pcb_t *first = removeProcQ(&(element->s_procQ));
        if (emptyProcQ(element->s_procQ)){
            // The process queue is now empty, deallocate
            // the semaphore descriptor
            prev->s_next = element->s_next;
            element->s_next = semdFree_h;
            semdFree_h = element;
        }

        return first;
    }

    return NULL;
}

pcb_t* outBlocked(pcb_t *p) {
    if (p == NULL) return NULL;

    // Get pointer to previous and actual semaphore
    // descriptor in ASL on which p is blocked
    semd_t *prev = findPrevSemd(p->p_semAdd);
    semd_t *element = prev->s_next;

    // Error condition: semaphore not in ASL
    if(element->s_semAdd != p->p_semAdd) return NULL;

    // Remove p from the queue of the semaphore descriptor
    pcb_t *removed = outProcQ(&(element->s_procQ), p);

    // If the queue becomes empty, remove the descriptor from ASL
    if (emptyProcQ(element->s_procQ)) {
        prev->s_next = element->s_next;
        element->s_next = semdFree_h;
        semdFree_h = element;
    }

    return removed;
}

pcb_t* headBlocked(int *semAdd) {
    semd_t *prev = findPrevSemd(semAdd);
    semd_t *element = prev->s_next;

    // If element->s_semAdd == semAdd, the
    // element was found
    if (element->s_semAdd == semAdd){
        return headProcQ(element->s_procQ);
    }
    return NULL;
}

void initASL() {
    // The first two elements of
    // semd_table contain the dummy head
    // and the dummy tail, respectively

    // Points to the first "real" element
    semdFree_h = &(semd_table[2]);

    // Create the free list from the "real"
    // elements
    for (int i = 2; i < MAXSEMD - 1; ++i) {
        semd_table[i].s_next = &(semd_table[i + 1]);
    }
    semd_table[MAXSEMD - 1].s_next = NULL;

    // Create the dummies
    semd_h = &semd_table[0];
    semd_h->s_semAdd = MININT;
    semd_h->s_next = &semd_table[1];
    
    semd_table[1].s_next = NULL;
    semd_table[1].s_semAdd = MAXPINT;
}


