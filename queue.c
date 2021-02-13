#include "h/queue.h"

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

//Berto----------------------------------
pcb_t* removeProcQ(pcb_t **tp){
    if(tp==NULL) return NULL; //se la coda è vuota ritorna NULL
    //altrimenti elimina il primo elemento e ritorna il puntatore ad esso
    else{
        //prendo il primo elemento della coda
        pcb_t *head = (*tp)->p_next;
        //Se la lista è composta da un solo elemento
        if(head->p_next==head){
            (*tp)=NULL;
        }else{
            //rendo il secondo elemento della lista il nuovo primo
            (*tp)->p_next = head->p_next;
            head->p_next->p_prev = (*tp);

        }
        //rendo prev e next null e ritorno l'elemento rimosso
        head->p_next=NULL;
        head->p_prev=NULL;
        return head;
    }
}
