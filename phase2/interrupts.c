#include <interrupts.h>
#include <syscalls.h>
#include <asl.h>
#include <pandos_const.h>
#include <pandos_types.h>
#include <umps3/umps/libumps.h>
#include <umps3/umps/cp0.h>
#define CAUSE_IP_GET(cause,line) (cause & CAUSE_IP_MASK) & CAUSE_IP(line)

unsigned startInterrupt, endInterrupt;

//Dichiarazione variabili
extern int processCount;
extern int softblockCount;
extern pcb_t* readyQueue;
extern pcb_t* currentProcess;
extern SEMAPHORE semaphoreList[];
extern SEMAPHORE semIntTimer;
extern unsigned endTimeSlice;
extern unsigned startTimeSlice;

int getDeviceNr(unsigned bitmap){
    int i;
    for (i = 0; i < 8; i++){
        if (bitmap == 1) return i;
        else bitmap = bitmap >> 1;
    }
    return -1;
}

void interruptHandler(state_t* excState){
    STCK(startInterrupt);
    unsigned int cause = (excState->cause);
    plis();
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
        plis();
        return; /* sollevato interrupt non riconosciuto */
    }
}


void dtpHandler(int type){
    int i, status, device_nr;
    dtpreg_t* dev;
    memaddr* interrupt_bitmap = (memaddr*) CDEV_BITMAP_ADDR(type);
    
    dev = (dtpreg_t*) DEV_REG_ADDR(type, device_nr);
    i = INSTANCES_NUMBER * (type - 3) + device_nr;
    status = dev->status;
    dev->command = ACK;

    softblockCount++;
    STCK(endInterrupt);
    pcb_t* free = removeBlocked(&semaphoreList[i]);
    free->p_time += (endInterrupt - startInterrupt);
    if(free!=NULL){;
        free->p_s.reg_v0 = status;
        insertProcQ(&readyQueue, free);
    }
    if(currentProcess != NULL){
        LDST(EXCTYPE);
    }else scheduler();
}

void terminalHandler(){
    int i, read, status, device_nr;
    termreg_t* term;
    memaddr* interrupt_bitmap = (memaddr*) CDEV_BITMAP_ADDR(INT_TERMINAL);

    term = (termreg_t*) DEV_REG_ADDR(INT_TERMINAL, device_nr);

    read = term->transm_status == READY;
    if (read){
        status = term->recv_status;
        term->recv_command = ACK;
    }
    else {
        status = term->transm_status;
        term->transm_command = ACK;
    }
    i = INSTANCES_NUMBER * (INT_TERMINAL + read - 3) + device_nr;
    
    softblockCount++;
    saveme();
    STCK(endInterrupt);
    pcb_t* free = removeBlocked(&semaphoreList[i]);
    free->p_time += (endInterrupt - startInterrupt);
    semaphoreList[i]++;
    if(free!=NULL){
        prova4();
        free->p_s.reg_v0 = status;
        insertProcQ(&readyQueue, free);
    }
    if(currentProcess != NULL){
        LDST(EXCTYPE);
    }else scheduler();
}

void PLTInterrupt(){
    setTIMER(10000000000);
    currentProcess->p_s = *EXCTYPE;
    currentProcess->p_time += 5000;
    insertProcQ(&readyQueue, currentProcess);
    currentProcess = NULL;
    scheduler();
}

void SWITInterrupt(){
    LDIT(SWTIMER);
    pcb_t *removedProcess = NULL;
    while(semIntTimer < 0){
        STCK(endInterrupt);
        removedProcess = removeBlocked(&semIntTimer);
        removedProcess->p_time += (endInterrupt - startInterrupt);
        insertProcQ(&readyQueue, removedProcess);
        softblockCount--;
        semIntTimer++;
    }
    if(currentProcess != NULL)
        LDST(EXCTYPE);
    else scheduler();
}

void saveme(){
    return;
}

void diocane2(){
    return;
}

void prova4(){
    return;
}

void plis(){
    return;
}