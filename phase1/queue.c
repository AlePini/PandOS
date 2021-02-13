#include <queue.h>

pcb_t *mkEmptyProcQ(){
    return NULL;
}

int emptyProcQ(pcb_t *tp){
    if(tp == NULL) return TRUE;
    return FALSE;
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

//TODO: ricontrolla
pcb_t *headProcQ(pcb_t *tp){
    if(tp == NULL) return NULL;
    return (*tp).p_next;
}

//PCBFREE -> primo

//dato che i pcb nella lista libera son tutti uguali a me basta trattarla come una pila
void freePcb(pcb_t* p){
    p->p_next=pcbFree_h;
    pcbFree_h=p;
}

pcb_t* resetPcb(pcb_t* p){
    p->p_next = NULL;
    p->p_prev = NULL;
    p->p_prnt = NULL;
    p->p_child = NULL;
    p->p_next_sib = NULL;
    p->p_prev_sib = NULL;
    return p;
}

pcb_t* allocPcb(){
    if(pcbFree_h == NULL) return NULL;
    pcb_t* toAlloc = pcbFree_h;
    pcbFree_h = pcbFree_h->p_next;
    return resetPcb(toAlloc);
}

void initPcbs(){
}

pcb_t *outProcQ(pcb_t **tp, pcb_t *p){
    return NULL;
}

pcb_t *removeProcQ(pcb_t **tp){
    return NULL;
}

pcb_t *outProcQ(pcb_t **tp, pcb_t *p){
    if(*tp == NULL || p == NULL) return NULL;
    //Se p è l'elemento in coda
    if(*tp == p){
        //Se la coda ha un solo elemento
        if((*tp)->p_next == *tp){
            *tp = NULL;
            return p;
        }
        (*tp)->p_prev->p_next = (*tp)->p_next;
        (*tp)->p_next->p_prev = (*tp)->p_prev;
        (*tp)=(*tp)->p_prev;
        return p;
    }
    //Se l'elemento si trova all'interno della lista
    else{
        pcb_t* head = (*tp)->p_next;
        while(head != (*tp)){
            //Se lo trovo
            if(head == p){
                head->p_prev->p_next = head->p_next;
                head->p_next->p_prev = head->p_prev;
                return p;
            }
            head = head->p_next;
        }
        return NULL;
    }
}
