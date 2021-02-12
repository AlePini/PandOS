#include "h/tree.h"
//Berto--------------------------------------
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
    //ritorno il figlio eliminato
    son->p_next_sib=NULL;
    son->p_prev_sib=NULL;
    son->p_prnt=NULL;
    return son;
}