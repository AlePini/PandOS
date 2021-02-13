#include "h/tree.h"

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

//Questa funzione rimuove il nodo dall'albero
pcb_t* trim(pcb_t *p){
    p->p_child = NULL;
    p->p_next_sib=NULL;
    p->p_prev_sib=NULL;
    p->p_prnt=NULL;
    return p;
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