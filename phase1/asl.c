#include <asl.h>

HIDDEN pcb_t* findPrevSem(int* semAdd){
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
    //Se c'è il semaforo con l'identificativo corretto inserisco il processo p
    if(next->s_semAdd == semAdd){
        p->p_semAdd  = semAdd;
        insertProcQ(&next->s_procQ, p);
    }else{//Altrimenti devo allocare un nuovo semaforo con l'identificativo giusto
        //Se la lista dei semafori liberi è vuota ritorno true
        if (semdFree_h == NULL) return TRUE;
        //Altrimenti ne alloco uno e lo inizializzo
        semd_t* toInsert = semdFree_h;
        semdFree_h = semdFree_h -> s_next;
        toInsert -> s_semAdd = semAdd;
        toInsert -> s_procQ = mkEmptyProcQ();
        p->p_semAdd  = semAdd;
        insertProcQ(&toInsert->s_procQ, p);
        //Poi lo inserisco nella lista dei semafori attivi
        toInsert -> s_next = head-> s_next;
        head -> s_next = toInsert;
    }
    return FALSE;
}

/**
    Removes the head of the procQ associated with the semaphore of the given identifier.
    If none is found, return NULL; otherwise, remove the first (i.e. head) pcb from
    the process queue and return a pointer to it.
*/
pcb_t* removeBlocked(int *semAdd){
    //se non trovo il sem ritorno NULL
    //rimuovo il primo processo dalla queue del semaforo e ritorno un puntatore ad esso
    //se rimuovendo il processo la coda diventa vuota rimuovo il semaforo e lo rimetto in semdFree
    semd_t* prev = findPrevSem(semAdd);
    semd_t* next = prev->s_next;
    if(head->s_next->s_semAdd == semAdd){
        pcb_t* removed = removeProcQ(&head->s_next->s_procQ);
        if(emptyProcQ(head->s_next->s_procQ)){
            //inserisci head in semdFree
            prev->s_next=next->s_next;
            next->s_semAdd = NULL;
            next->s_next=semdFree_h;
            semdFree_h=next;
        }
        return removed;
    }
    return NULL;
}

/**
    Removes the PCBb pointed by p from the process queue associated
    with p’s semaphore.
    If pcb pointed by p does not appear in the process queue associated with p’s
    semaphore return NULL; otherwise, return p.
*/
pcb_t* outBlocked(pcb_t *p) {
    if (p == NULL) return NULL;//TODO: serve davvero?

    semd_t* prev = findPrevSem(semAdd);
    semd_t* next = prev->s_next;

    if(next->s_semAdd != p->p_semAdd) return NULL;
    pcb_t *toRemove = outProcQ(&(next->s_procQ), p);

    if(emptyProcQ(next->s_procQ)){
        //inserisci head in semdFree
        prev->s_next=next->s_next;
        next->s_next=semdFree_h;
        semdFree_h=next;
    }
    return toRemove;
}


/**
    Returns a pointer to the pcb  head of the process queue
    associated with the given semaphore. Returns NULL if semAdd is
    not found or if its process is empty.
 */
pcb_t* headBlocked(int *semAdd){
    //NULL se semAdd is not found o se la process queue di semAdd è vuota
    //ritorna il puntatore al primo processo delle queue di semAdd
    semd_t* prev = findPrevSem(semAdd);
    semd_t* next = prev->s_next;

    if(next->s_semAdd != semAdd)
        return NULL;

    return headProcQ(next->s_procQ);
}

/**
    Initializes the semdFree list.
*/
void initASL(){

    //Parto aggiungendo i due semafori extra nella lista di quelli attivi
    //Quello con identificativo pari a MINSEM che è il primo della table
    semd_h = &semd_table[0];
    semd_h -> s_semAdd = MININT;
    semd_h -> s_procQ = NULL;
    //E quello con identificativo pari a MAXINT che è il secondo della table
    semd_h ->s_next = &semd_table[1];
    semd_h ->s_next->s_semAdd = MAXINT;
    semd_h ->s_next->s_procQ = NULL;
    //La faccio terminare con NULL
    semd_h->s_next->s_next = NULL;

    //Associo alla lista dei semafori liberi al primo disponibile nella table
    semdFree_h = &semd_table[2];
    semd_t* tmp = semdFree_h;
    //Aggiungo alla lista semdFree tutti gli altri fino ad averne un numero pari a MAXPROC
    for(int i=3; i<MAXSEM; i++){
        tmp->s_next = &semd_table[i];
        tmp = tmp->s_next;
    }
    //La faccio terminare con NULL
    tmp->s_next = NULL;
}


