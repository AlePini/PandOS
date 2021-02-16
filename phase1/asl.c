#include "h/asl.h"

int insertBlocked(int *semAdd, pcb_t *p){

    semd_t* head = semd_h;
    //Assegno a p il suo semAdd, lo faccio ora per non doverlo scrivere due volte poi nell'if-else
    p->p_semAdd  = semAdd;
    //Arrivo alla posizione corretta della lista dei semafori
    while(*(head ->s_next -> s_semAdd) < semAdd){
        head = head->s_next;
    }
    //Se c'è il semaforo con l'identificativo corretto inserisco il processo p
    if(*(head -> s_semAdd) == semAdd){
            insertProcQ(&head->s_procQ, p);
    }else{//Altrimenti devo allocare un nuovo semaforo con l'identificativo giusto
        //Se la lista dei semafori liberi è vuota ritorno true
        if (semdFree_h == NULL) return TRUE;
        //Altrimenti ne alloco uno e lo inizializzo
        semd_t* toInsert = semdFree_h;
        semdFree_h = semdFree_h -> s_next;
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
    //se non trovo il sem ritorno NULL
    //rimuovo il primo processo dalla queue del semaforo e ritorno un puntatore ad esso
    //se rimuovendo il processo la coda diventa vuota rimuovo il semaforo e lo rimetto in semdFree
    semd_t* head=semd_h;
    while(*(head->s_next->s_semAdd)!=MAXINT){
        head=head->s_next;
        if(*(head->s_next->s_semAdd)==semAdd){
            pcb_t* tmp=removeProcQ(head->s_next->s_procQ);
            if(emptyProcQ(head->s_next->s_procQ)){
                //inserisci head in semdFree
                semd_t* toRemove=head->s_next;
                head->s_next=head->s_next->s_next;
                toRemove->s_next=semdFree_h;
                semdFree_h=toRemove;
            }
            return tmp;
        }
    }
    return NULL;
}

pcb_t* outBlocked(pcb_t *p){
    semd_t* head = semd_h;
    int semAdd = *(p -> p_semAdd);

    while ( *(head -> s_semAdd) != semAdd){
        head = head -> s_next;
    }

    return outProcQ(&head -> s_procQ, p);
}

pcb_t* headBlocked(int *semAdd){
    //NULL se semAdd is not found o se la process queue di semAdd è vuota
    //ritorna il puntatore al primo processo delle queue di semAdd
    semd_t* head=semd_h;
    while (*(head->s_semAdd)!=MAXINT){
        head=head->s_next;
        if(*(head->s_semAdd)==semAdd)
            return headProcQ(head->s_procQ);
    }
    return NULL;
}

void initASL(){
    //Associo alla lista dei semafori liberi al primo disponibile nella table
    semdFree_h = &semd_table[1];
    semd_t* tmp = semdFree_h;
    semd_t* tmp2 = semd_h;
    //Aggiungo alla lista semdFree tutti gli altri fino ad averne un numero pari a MAXPROC
    for(int i=2; i<MAXSEM-1; i++){
        tmp->s_next = &semd_table[i];
        tmp = tmp->s_next;
    }
    //La faccio terminare con NULL
    tmp->s_next = NULL;
    //Vado poi ad aggiungere due semafori nella lista di quelli attivi
    //Quello con identificativo pari a 0 che è il primo della table
    tmp2 = &semd_table[0];
    tmp2 -> s_semAdd = MINSEM;
    //E quello con identificativo pari a MAXINT che è l'ultimo della table
    tmp2 ->s_next = &semd_table[MAXSEM-1];
    tmp2 ->s_semAdd = MAXINT;
    //La faccio terminare con NULL
    tmp2 = tmp2 -> s_next;
    tmp2-> s_next = NULL;
}


