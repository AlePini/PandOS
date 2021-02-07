#include "queue.h";

void head_insert(pcb_t* list, pcb_t* elem)
{
    if(elem == NULL) return;
    if(list == NULL) list = elem;

    //DA RIGUARDARE QUESTO CASO
    if(list->p_next == NULL)
    {
        list->p_next == elem;
        list->p_prev == elem;
        elem->p_next = list;
        elem->p_prev = list;
    }
    else
    {
        elem->p_prev = list;
        elem->p_next = list->p_next;
        list->p_next = elem;
        elem->p_next->p_prev = elem;
    }
}

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


pcb_t *headProcQ(pcb_t **tp){
    if(*tp == NULL) return NULL;
    return (**tp).p_next;
}

//PCBFREE -> primo

//dato che i pcb nella lista libera son tutti uguali a me basta trattarla come una pila
void freePcb(pcb_t* p){
    p->p_next=pcbFree_h;
    pcbFree_h=p;
}

pcb_t* resetPcb(pcb_t* p){
    p->p_next = NULL;
    p->p_prev = NULL;+
    p->p_prnt = NULL;
    p->p_child = NULL;
    p->p_next_sib = NULL;
    p->p_prev_sib = NULL;
    //TODO: da mettere a posto
    //p->p_s = 0;
    return p;
}

pcb_t* allocPcb(){
    if(pcbFree_h == NULL) return NULL;
    pcb_t* toAlloc = pcbFree_h;
    pcbFree_h = pcbFree_h->p_next;
    return resetPcb(toAlloc);
}
