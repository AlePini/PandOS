#include <pcb.h>

//---------- QUEUE ----------

//dato che i pcb nella lista libera son tutti uguali a me basta trattarla come una pila
void freePcb(pcb_t* p){
    p->p_next=pcbFree_h;
    pcbFree_h=p;
}

pcb_t* allocPcb(){
    if(pcbFree_h == NULL) return NULL;
    pcb_t* toAlloc = pcbFree_h;
    pcbFree_h = pcbFree_h->p_next;
    return resetPcb(toAlloc);
}

void initPcbs(){

    pcbFree_h = &pcbFree_table[0];

    pcb_t* tmp = pcbFree_h;

    for(int i = 1; i < MAXPROC; i++){

        tmp -> p_next = &pcbFree_table[i];
        tmp = tmp-> p_next;
    }

    tmp -> p_next = NULL;
}


pcb_t *mkEmptyProcQ(){
    return NULL;
}

int emptyProcQ(pcb_t *tp){
    return tp == NULL;
}

void insertProcQ(pcb_t **tp, pcb_t *p){

    //Se tp ==  NULL vuol dire che non esiste la lista di conseguenza dopo ci sarà solo p
    if(*tp == NULL){
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

pcb_t* removeProcQ(pcb_t **tp){
    if(*tp==NULL) return NULL; //se la coda è vuota ritorna NULL
    //altrimenti elimina il primo elemento e ritorna il puntatore ad esso
    else{
        //prendo il primo elemento della coda
        pcb_t *head = (*tp)->p_next;
        //Se la lista è composta da un solo elemento
        if(head->p_next==head){
            *tp=NULL;
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

pcb_t *outProcQ(pcb_t **tp, pcb_t *p){
    if(*tp == NULL || p == NULL) return NULL;
    //Se p è l'elemento in coda
    if(*tp == p){
        //Se la coda ha un solo elemento
        if((*tp)->p_next == *tp){
            *tp = NULL;
            return p;
        }
        //Se l'elemento è quello che si trova in coda
        (*tp)->p_prev->p_next = (*tp)->p_next;
        (*tp)->p_next->p_prev = (*tp)->p_prev;
        (*tp)=(*tp)->p_prev;
        return p;
    }
    //Se l'elemento si trova all'interno della lista
    else{
        //Prendo la testa e scorro finchè non torno in coda
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

//TODO: ricontrolla
pcb_t *headProcQ(pcb_t *tp){
    if(tp == NULL) return NULL;
    return tp->p_next;
}

pcb_t* resetPcb(pcb_t* p){
    p->p_next = NULL;
    p->p_prev = NULL;
    p->p_prnt = NULL;
    p->p_child = NULL;
    p->p_next_sib = NULL;
    p->p_prev_sib = NULL;
    p->p_semAdd = NULL;
    return p;
}

//---------- TREE ----------

//IMPORTANTE SPECIFICARE CHE LA LISTA DEI FIGLI SIA DOPPIA MA NON CIRCOLARE

int emptyChild(pcb_t *p){
    if (p->p_child == NULL) return TRUE;
    return FALSE;
}

void insertChild(pcb_t *prnt, pcb_t *p){

    //inserisco p come figlio di prnt se non è vuoto
    if(p!=NULL){
        if(prnt->p_child==NULL){//se prnt non ha figli inserisco p come figlio
            prnt->p_child=p;
            p->p_prnt=prnt;
        }else{
            p->p_next_sib = prnt->p_child;
            prnt->p_child->p_prev_sib=p;
            prnt->p_child = p;
            p->p_prnt=prnt;
        }
    }
}

pcb_t* removeChild(pcb_t *p){
    if(p==NULL || p->p_child==NULL) return NULL;

    //individuo il primo filgio di p
    pcb_t* son=p->p_child;
    if(son->p_next_sib==NULL){  // è figlio unico
        p->p_child=NULL;
    }else{                      //ha almeno 1 fratello
        p->p_child=son->p_next_sib;
        p->p_child->p_prev_sib=NULL;
    }

    return trim(son);
}

pcb_t *outChild(pcb_t *p){
    if (p->p_prnt==NULL)
        return NULL;

    //Se p non ha un fratello sinistro allora è il primo figlio del suo parent, sfrutto removeChild.
    if (p->p_prev_sib == NULL)
        return removeChild(p->p_prnt);

    //Elimino p dalla lista dai fratelli
    p->p_prev_sib->p_next_sib=p->p_next_sib;
    //Solo se p non è l'ultimo fratello della lista devo far si che il fratello sinistro di p diventi il fratello sinistro del fratello destro di p.
    if(p->p_next_sib != NULL )  p->p_next_sib->p_prev_sib=p->p_prev_sib;

    return trim(p);
}

//Questa funzione rimuove il nodo dall'albero
pcb_t* trim(pcb_t *p){
    p->p_child = NULL;
    p->p_next_sib=NULL;
    p->p_prev_sib=NULL;
    p->p_prnt=NULL;
    return p;
}