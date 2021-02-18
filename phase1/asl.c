#include <asl.h>

//This function takes in input a semAdd and return the semaphore with
//the identifier of maximum value but lower than the one given as argument.
HIDDEN semd_t* findPrevSem(int* semAdd){
    semd_t* head = semd_h;
    while(head->s_next->s_semAdd < semAdd){
        head = head->s_next;
    }
    return head;
}

/*
    Inserts PCB p at the tail of the process queue associated
    with the semaphore whose physical address is given.
    If the semaphore is currently not active, allocate a new descriptor from
    the semdFree list, initialize all of its fields and insert it in the ASL.
    If a new semaphore descriptor needs to be allocated and the
    semdFree list is empty, return TRUE. Otherwise FALSE.
*/
int insertBlocked(int *semAdd, pcb_t *p){
    if (p == NULL) return FALSE;        //TODO: serve davvero?
    semd_t* prev = findPrevSem(semAdd);
    semd_t* next = prev->s_next;
    //If there's the semaphore with the correct identifier inserts the process p.
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

/**
    Removes the head of the procQ associated with the semaphore of the given identifier.
    If none is found, return NULL; otherwise, remove the first (i.e. head) pcb from
    the process queue and return a pointer to it.
*/
pcb_t* removeBlocked(int *semAdd){

    semd_t* prev = findPrevSem(semAdd);
    semd_t* next = prev->s_next;

    //If the semaphore the function is looking for is not there returns NULL.
    if(next->s_semAdd != semAdd) return NULL;

    //Otherwise removes the head of the queue of the processes.
    pcb_t* removed = removeProcQ(&next->s_procQ);
    //If by doing so the processes' queue becomes empty, deallocates the semaphore.
    if(emptyProcQ(next->s_procQ)){
        //Returns the empty semaphore to the semdFree.
        prev->s_next=next->s_next;
        next->s_semAdd = NULL;
        next->s_next=semdFree_h;
        semdFree_h=next;
    }
    return removed;
}

/**
    Removes the PCBb pointed by p from the process queue associated
    with p’s semaphore.
    If pcb pointed by p does not appear in the processes' queue associated with p’s
    semaphore return NULL; otherwise, returns p.
*/
pcb_t* outBlocked(pcb_t *p) {

    if (p == NULL) return NULL;//TODO: serve davvero?
    semd_t* prev = findPrevSem(p->p_semAdd);
    semd_t* next = prev->s_next;

    //If the semaphore is not there returns NULL.
        if(next->s_semAdd != p->p_semAdd) return NULL;

    //Otherwise removes p from the processes' queue.
    pcb_t *toRemove = outProcQ(&(next->s_procQ), p);
    //If by doing so the processes' queue becomes empty, deallocates the semaphores.
    if(emptyProcQ(next->s_procQ)){
        //Returns the empty semaphores to the semdFree.
        prev->s_next=next->s_next;
        next->s_semAdd = NULL;
        next->s_next=semdFree_h;
        semdFree_h=next;
    }
    return toRemove;
}


/**
    Returns a pointer to the pcb  head of the processes' queue
    associated with the given semaphore. Returns NULL if semAdd is
    not found or if its process is empty.
 */
pcb_t* headBlocked(int *semAdd){
    semd_t* prev = findPrevSem(semAdd);
    semd_t* next = prev->s_next;
    //If the semaphores is not there returns NULL.
    if(next->s_semAdd != semAdd)
        return NULL;
    //Otherwise returns the head of the processes' queue.
    return headProcQ(next->s_procQ);
}

/**
    Initializes the semdFree list.
*/
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


