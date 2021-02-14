#include "h/asl.h"

int insertBlocked(int *semAdd, pcb_t *p){
    return 0;
}

pcb_t* removeBlocked(int *semAdd){
    return NULL;
}

pcb_t* outBlocked(pcb_t *p){
    return NULL;
}

pcb_t* headBlocked(int *semAdd){
    return NULL;
}

void initASL(){

    semdFree_h = &semd_table[1];
    semd_t* tmp = semdFree_h;
    for(int i=2; i<MAXSEM-1; i++){
        tmp->s_next = &semd_table[i];
        tmp = tmp->s_next;
    }

    semd_h = &semd_table[0];
    semd_h -> s_semAdd = MINSEM;

    semd_h ->s_next = &semd_table[MAXSEM-1];
    semd_h ->s_semAdd = MAXINT;
}