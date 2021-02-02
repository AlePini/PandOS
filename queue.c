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