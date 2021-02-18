#include <asl.h>

//Questa funzione prende in input un semAdd e ritorna il semaforo con
//l'identificativo di valore massimo ma minore di quello passato come argomento
HIDDEN semd_t* findPrevSem(int* semAdd){
    semd_t* head = semd_h;
    while(head->s_next->s_semAdd < semAdd){
        head = head->s_next;
    }
    return head;
}

int insertBlocked(int *semAdd, pcb_t *p){
    if (p == NULL) return FALSE;        //TODO: serve davvero?
    semd_t* prev = findPrevSem(semAdd);
    semd_t* next = prev->s_next;
    //Se c'è il semaforo con l'identificativo corretto inserisco il processo p
    if(next->s_semAdd == semAdd){
        p->p_semAdd  = semAdd;
        insertProcQ(&next->s_procQ, p);
    }
    //Altrimenti devo allocare un nuovo semaforo con l'identificativo giusto
    else{
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
        toInsert -> s_next = prev-> s_next;
        prev -> s_next = toInsert;
    }
    return FALSE;
}

pcb_t* removeBlocked(int *semAdd){

    semd_t* prev = findPrevSem(semAdd);
    semd_t* next = prev->s_next;

    //Se il semaforo che cerco non è presente ritorno NULL
    if(next->s_semAdd != semAdd) return NULL;

    //Altrimenti rimuovo la testa della coda dei processi
    pcb_t* removed = removeProcQ(&next->s_procQ);
    //Se facendolo la coda dei processi diventa vuota vado a deallocare il semaforo
    if(emptyProcQ(next->s_procQ)){
        //ritorno il semaforo vuoto alla semdFree
        prev->s_next=next->s_next;
        next->s_semAdd = NULL;
        next->s_next=semdFree_h;
        semdFree_h=next;
    }
    return removed;
}

pcb_t* outBlocked(pcb_t *p) {

    if (p == NULL) return NULL;//TODO: serve davvero?
    semd_t* prev = findPrevSem(p->p_semAdd);
    semd_t* next = prev->s_next;

    //Se il semaforo che cerco non è presente ritorno NULL
    if(next->s_semAdd != p->p_semAdd) return NULL;

    //Altrimenti ne estraggo p dalla coda dei processi
    pcb_t *toRemove = outProcQ(&(next->s_procQ), p);
    //Se facendolo la coda dei processi diventa vuota vado a deallocare il semaforo
    if(emptyProcQ(next->s_procQ)){
        //ritorno il semaforo vuoto alla semdFree
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
    //Se il semaforo che cerco non è presente ritorno NULL
    if(next->s_semAdd != semAdd)
        return NULL;
    //Altrimenti ne ritorno la testa della coda dei processi
    return headProcQ(next->s_procQ);
}


void initASL(){

    //Parto aggiungendo i due semafori extra nella lista di quelli attivi
    //Quello con identificativo pari a MINSEM che è il primo della table
    semd_h = &semd_table[0];
    semd_h -> s_semAdd = MININT;
    //E quello con identificativo pari a MAXINT che è il secondo della table
    semd_h ->s_next = &semd_table[1];
    semd_h ->s_next->s_semAdd = MAXINT;
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


