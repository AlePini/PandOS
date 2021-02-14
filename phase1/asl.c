#include <asl.h>

int insertBlocked(int *semAdd,pcb_t *p){
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

    semdFree_h = &semd_table[0];
    semd_t* tmp = semdFree_h;
    for(int i=1; i<MAXPROC; i++){
        tmp->s_next = &semd_table[i];
        tmp = tmp->s_next;
    }
    semd_t* end;
    end -> s_semAdd = 0;
    end -> s_procQ = MAXINT;
    tmp->s_next = end;
    semd_t* init;
    init -> s_semAdd = 0;
    init -> s_procQ = NULL;
    init -> s_next = semdFree_h;
    semdFree_h = init;
}