#include "h/asl.h"

semd_t* allocSem(){
    semd_t* toAlloc = semdFree_h;
    semdFree_h = semdFree_h -> s_next;
    return toAlloc;
}

int insertBlocked(int *semAdd, pcb_t *p){

    semd_t* head = semd_h;
    //Assegno a p il suo semAdd, lo faccio ora per non doverlo scrivere due volte poi nell'if-else
    p->p_semAdd  = semAdd;
    //Arrivo alla posizione corretta della lista dei semafori
    while(head ->s_next -> s_semAdd < semAdd){
        head = head->s_next;
    }
    //Se c'è il semaforo con l'identificativo corretto inserisco il processo p
    if(head -> s_semAdd == semAdd){
            insertProcQ(&head->s_procQ, p);
    }else{//Altrimenti devo allocare un nuovo semaforo con l'identificativo giusto
        //Se la lista dei semafori liberi è vuota ritorno true
        if (semdFree_h == NULL) return TRUE;
        //Altrimenti ne alloco uno e lo inizializzo
        semd_t* toInsert = allocSem();
        toInsert -> s_semAdd = semAdd;
        toInsert -> s_procQ = makeEmptyProcQ();
        insertProcQ(&head->s_procQ, p);
        //Poi lo inserisco nella lista dei semafori attivi
        toInsert -> s_next = head-> s_next;
        head -> s_next = toInsert;
    }
    return FALSE;
}

pcb_t* removeBlocked(int *semAdd){
    return NULL;
}

pcb_t* outBlocked(pcb_t *p){
    return NULL;
}

pcb_t* headBlocked(int *semAdd){
    return NULL;
}

void initASL(){
    //Associo alla lista dei semafori liberi al primo disponibile nella table
    semdFree_h = &semd_table[1];
    semd_t* tmp = semdFree_h;
    //Aggiungo alla lista semdFree tutti gli altri fino ad averne un numero pari a MAXPROC
    for(int i=2; i<MAXSEM-1; i++){
        tmp->s_next = &semd_table[i];
        tmp = tmp->s_next;
    }
    //Vado poi ad aggiungere due semafori nella lista di quelli attivi
    //Quello con identificativo pari a 0 che è il primo della table
    semd_h = &semd_table[0];
    semd_h -> s_semAdd = MINSEM;
    //E quello con identificativo pari a MAXINT che è l'ultimo della table
    semd_h ->s_next = &semd_table[MAXSEM-1];
    semd_h ->s_semAdd = MAXINT;
}