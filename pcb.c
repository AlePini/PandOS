#include "pandos_types.h"
#include "pandos_const.h"

static pcb_t pcbFree_table[MAXPROC];
pcbFree *pcbFree_h;

void initPcbs(){

    pcbFree_h->p = NULL;
    pcbFree_h->pcb_next = NULL;

    for (int i = 0; i < MAXPROC; i++){
        freePcb(&pcbFree_table[i]);
    }
    
}

void freePcb(pcb_t *p){

    if(pcbFree_h->p == NULL){
        pcbFree_h->p = p;
        return;
    }

    /* insert in head */

    pcbFree *tmp;
    tmp->p = p;
    tmp->pcb_next = pcbFree_h;

    pcbFree_h = tmp;

}

pcb_t *allocPcb(){
    return NULL; // return pointer allocated
}