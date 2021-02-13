#include "h/queue.h";

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
        *tp = p;
        p->p_prev = p;
        p->p_next = p;
        return;
    }

    p->p_prev=*tp;                  //Il precedente di p diventa tp
    p->p_next = (*tp)->p_next;      //il prossimo di p sarà quello che era il next di tp
    p->p_prev->p_next = p;          //p->p_prev è tp. il next di tp diventa p.
    p->p_next->p_prev = p;          //il next di p è il primo. Il prev del primo è p
    *tp = p;                        //sposto tp su p
}

//TODO: ricontrolla
pcb_t *headProcQ(pcb_t *tp){
    if(tp == NULL) return NULL;
    return tp->p_next;
}

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
