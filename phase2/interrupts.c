#include <interrupts.h>

#define CAUSE_IP_GET(cause,line) (cause & CAUSE_IP_MASK) & CAUSE_IP(line)

int getDeviceNr(unsigned bitmap){
    int i;
    for (i = 0; i < 8; i++){
        if (bitmap == 1) return i;
        else bitmap = bitmap >> 1;
    }
    return -1;
}

void interruptHandler(){
    state_t *exceptionState = EXCTYPE;
    unsigned int cause = exceptionState->cause.IP;

    if(CAUSE_IP_GET(cause, INT_PLT)){
        PLTInterrupt();
    } else if(CAUSE_IP_GET(cause, INT_SWT)){
        SWITInterrupt();
    } else if(CAUSE_IP_GET(cause, INT_DISK)){
        dtpHandler(INT_DISK);
    } else if(CAUSE_IP_GET(cause, INT_TAPE)){
        dtpHandler(INT_TAPE);
    } else if(CAUSE_IP_GET(cause, INT_NETWORK)){
        dtpHandler(INT_NETWORK);
    } else if(CAUSE_IP_GET(cause, INT_PRINTER)){
        dtpHandler(INT_PRINTER);
    } else if(CAUSE_IP_GET(cause, INT_TERMINAL)){
        terminalHandler();
    } else {
        PANIC(); /* sollevato interrupt non riconosciuto */
    }
}


void dtpHandler(int type){
    int i, status, device_nr;
    dtpreg_t* dev;
    memaddr* interrupt_bitmap = (memaddr*) CDEV_BITMAP_ADDR(type);

    if ((device_nr = getDeviceNr(*interrupt_bitmap)) < 0) PANIC();
    else dev = (dtpreg_t*) DEV_REG_ADDR(type, device_nr);
    i = DEV_PER_INT * (type - 3) + device_nr;
    status = dev->status;
    dev->command = CMD_ACK;
    pcb_t* free = verhogen(&dev_sem[i]);
    free->p_s.reg_v0 = status;
    if(free!=NULL)
        insertProcQ(&readyQueue, free);
    if(currentProcess != NULL)
        LDST(EXCTYPE);
}

void terminalHandler(){
    int i, status, device_nr;
    termreg_t* term;
    memaddr* interrupt_bitmap = (memaddr*) CDEV_BITMAP_ADDR(INT_TERMINAL);

    if ((device_nr = getDeviceNr(*interrupt_bitmap)) < 0) PANIC();
    else term = (termreg_t*) DEV_REG_ADDR(INT_TERMINAL, device_nr);

    if ((term->recv_status & TERM_STATUS_MASK) == ST_TRANS_RECV){
        i = DEV_PER_INT * (INT_TERMINAL - 3) + device_nr;
        status = term->recv_status;
        term->recv_command = CMD_ACK;
    }
    else if ((term->transm_status & TERM_STATUS_MASK) == ST_TRANS_RECV){
        i = DEV_PER_INT * (INT_TERMINAL - 3 + 1) + device_nr;
        status = term->transm_status;
        term->transm_command = CMD_ACK;
    }

    pcb_t* free = verhogen(&dev_sem[i]);
    free->p_s.reg_v0 = status;
    if(free!=NULL)
        insertProcQ(&readyQueue, free);
    if(currentProcess != NULL)
        LDST(EXCTYPE);
}

void PLTInterrupt(){
    setTIMER(PLTTIMER);
    currentProcess->p_s = EXCTYPE;
    currentProcess->p_time += 5000;
    insertProcQ(&readyQueue, currentProcess);
    currentProcess = NULL;
    scheduler();
}

void SWITInterrupt(){
    LDIT(SWTIMER);
    pcb_t* removedProcess;
    do{
        removedProcess; = removeBlocked()
    }while(removed != NULL);
    (*semaddr) = 0;
    if(currentProcess != NULL)
        LDST(EXCTYPE);
}
