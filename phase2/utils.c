#include <utils.h>;

HIDDEN pcb_t* createFirstProcess(memaddr func){
    pcb_t p = initializeProcess(allocPcb());
    return initializeState(p, func);
}

HIDDEN pcb_t* initializeProcess(pcb_t p){
    //Setting default values for all fields
    p->p_child = NULL;
    p->p_prnt = NULL;
    p->p_next_sib = NULL;
    p->p_prev_sib = NULL;
    p->p_time = 0;
    p->p_semAdd = NULL;
    p->p_supportStruct = NULL;
    return p;
}

//IGNORARE STA ROBA è da RIFARE
HIDDEN pcb_t* initializeState(pcb_t* p, memaddr func){
    state_t p_s;
    STST(&p_s);

    p_s->s_sp = RAMTOP; //Assegna lo stack pointer a RAMTOP
    p_s->s_pc = func;   // Imposta pc alla funzione test
    LDST(&p->p_s);
    return p;
}


