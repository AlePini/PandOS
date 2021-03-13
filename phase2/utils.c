#include <utils.h>;

HIDDEN pcb_t* createFirstProcess(){
    pcb_t p = initializeProcess(allocPcb());
    return initializeState(p);
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
}

//IGNORARE STA ROBA è da RIFARE
HIDDEN pcb_t* initializeState(pcb_t* p){
    state_t p_s;
    STST(&p_s);
    // Imposta lo status del processo
    p_s->status = p_s->s_status & ~(STATUS_VMc | STATUS_VMp | STATUS_VMo); /* Virtual memory OFF */
    p_s->status = p_s->s_status & ~(STATUS_KUc | STATUS_KUp | STATUS_KUo); /* Kernel mode ON */
    p_s->status = p_s->s_status | STATUS_TE; /* Timer ON */
    p_s->status = p_s->s_status | STATUS_IEc | STATUS_IEp | STATUS_IEo; /* Interrupt abilitati */
    p_s->status = p_s->s_status | STATUS_IM_MASK; /* Attiva tutti gli interrupt */
    p_s->reg_sp = RAMTOP;    //Credo
    p_s->pc_epc = entry_point; /* Imposta pc all'entry point */
    p->p_s;
    return p;
}
