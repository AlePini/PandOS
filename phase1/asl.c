#include <asl.h>

/**This function works also when the semaphore we're searching for is not present in the ASL.
*The function will return the last semaphore before the one whose identifier is MAXINT and
*the checks in the other functions will handle error situations.
*/

HIDDEN semd_t* findPrevSem(int* semAdd){
    //Scan the ASL until the next semaphore identifier is bigger than semAdd
    semd_t* head = semd_h;
    while(head->s_next->s_semAdd < semAdd){
        head = head->s_next;
    }
    return head;
}

int insertBlocked(int *semAdd, pcb_t *p){
    //Checks if the PCB pointer passed is not NULL
    if (p == NULL) return FALSE;

    semd_t* prev = findPrevSem(semAdd);
    semd_t* next = prev->s_next;
    //If the semaphore with the correct identifier is found the function inserts the process p.
    if(next->s_semAdd == semAdd){
        p->p_semAdd  = semAdd;
        insertProcQ(&next->s_procQ, p);
    }
    //Otherwise it has to allocate a new semaphore with the right identifier.
    else{

        //If the list of the free semaphores is empty returns true.
        if (semdFree_h == NULL) return TRUE;

        //Otherwise allocates and initializes one.
        semd_t* toInsert = semdFree_h;
        semdFree_h = semdFree_h -> s_next;
        toInsert -> s_semAdd = semAdd;
        toInsert -> s_procQ = mkEmptyProcQ();
        p->p_semAdd  = semAdd;
        insertProcQ(&toInsert->s_procQ, p);

        //Then inserts it in the list of the actives semaphores.
        toInsert -> s_next = prev-> s_next;
        prev -> s_next = toInsert;
    }
    return FALSE;
}

pcb_t* removeBlocked(int *semAdd){

    semd_t* prev = findPrevSem(semAdd);
    semd_t* next = prev->s_next;

    //If the semaphore the function is looking for is not there returns NULL.
    if(next->s_semAdd != semAdd) return NULL;

    //Otherwise removes the head of the queue of the processes.
    pcb_t* removed = removeProcQ(&next->s_procQ);
    //If by doing so the process queue becomes empty, deallocates the semaphore.
    if(emptyProcQ(next->s_procQ)){
        //Returns the empty semaphore to the semdFree.
        prev->s_next=next->s_next;
        next->s_semAdd = NULL;
        next->s_next=semdFree_h;
        semdFree_h=next;
    }
    return removed;
}

pcb_t* outBlocked(pcb_t *p) {
    //Checks if the PCB pointer passed is not NULL
    if (p == NULL) return NULL;
    semd_t* prev = findPrevSem(p->p_semAdd);
    semd_t* next = prev->s_next;

    //If the semaphore is not in the ASL returns NULL.
    if(next->s_semAdd != p->p_semAdd) return NULL;
    //Otherwise removes p from the process queue.
    pcb_t *toRemove = outProcQ(&(next->s_procQ), p);
    //If by doing so the process queue becomes empty, deallocates the semaphores.
    if(emptyProcQ(next->s_procQ)){
        //Returns the empty semaphores to the semdFree.
        prev->s_next=next->s_next;
        next->s_semAdd = NULL;
        next->s_next=semdFree_h;
        semdFree_h=next;
    }
    return toRemove;
}

pcb_t* headBlocked(int *semAdd){
    semd_t* prev = findPrevSem(semAdd);
    semd_t* next = prev->s_next;
    //If the semaphores is not in the ASL returns NULL.
    if(next->s_semAdd != semAdd)
        return NULL;
    //Otherwise returns the head of the process queue.
    return headProcQ(next->s_procQ);
}


void initASL(){

    //Adds the two extra sempaphores of the actives ones' list.
    //The one with the identifier = MININT which is the first of the table.
    semd_h = &semd_table[0];
    semd_h -> s_semAdd = MININT;
    //The one with the identifier = MAXINT which is the second of the table.
    semd_h ->s_next = &semd_table[1];
    semd_h ->s_next->s_semAdd = MAXINT;
    //Ends with NULL.
    semd_h->s_next->s_next = NULL;

    //Associo alla lista dei semafori liberi al primo disponibile nella table
    semdFree_h = &semd_table[2];
    semd_t* tmp = semdFree_h;
    //Adds to the semdFree list all the other semaphores until their number is equal to MAXPROC.
    for(int i=3; i<MAXSEM; i++){
        tmp->s_next = &semd_table[i];
        tmp = tmp->s_next;
    }
    //Ends with NULL.
    tmp->s_next = NULL;
}


