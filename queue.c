#include "queue.h";

pcb_t *mkEmptyProcQ(){
    return NULL;
}

int emptyProcQ(pcb_t *tp){
    if(tp == NULL) return TRUE;
    return FALSE;
}

pcb_t *headProcQ(pcb_t **tp){
    if(*tp == NULL) return NULL;
    return (**tp).p_next;
}

void insertProcQ(pcb_t **tp, pcb_t *p){

    //Se tp ==  NULL vuol dire che non esiste la lista di conseguenza dopo ci sarà solo p
    if(tp == NULL){
        (*tp) = p;
        (**tp).p_prev = (*tp);
        (**tp).p_next = (*tp);
        return;
    }

    (*p).p_prev=(**tp).p_prev;      //quello prima di p sarà il prec di tp
    (*p).p_next = *tp;              //il prossimo di p sarà quello che ora è tp
    (*((*p).p_prev)).p_next = p;    //il next del prev di p deve essere p
    (**tp).p_prev = p;              //il prev di tp sarà p
    *tp = p;                        //sposto tp su p

}