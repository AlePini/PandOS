#include <umps3/umps/cp0.h>
#include <umps3/umps/types.h>
#include <umps3/umps/libumps.h>
#include <umps3/umps/arch.h>
#include <pandos_const.h>
#include <asl.h>
#include <pcb.h>
#include <exceptions.h>
#include <initial.h>
#include <interrupts.h>
#include <scheduler.h>
#include <syscalls.h>

HIDDEN cpu_t startInterrupt, endInterrupt;
HIDDEN volatile int status;

/**
 * @brief Return the device number of the interrupt line.
 */
HIDDEN int getDeviceNr(unsigned bitmap){
    int i;
    for (i = 0; i < 8; i++){
        if (bitmap == 1) return i;
        else bitmap = bitmap >> 1;
    }
    return -1;
}



HIDDEN void returnControl(){
    if(currentProcess == NULL){
        scheduler();
    }else LDST(EXCEPTION_STATE);
}

HIDDEN void exitDeviceInterrupt(int i){
    STCK(endInterrupt);
    pcb_t* free = verhogen(&semaphoreList[i]);
    free->p_time += (endInterrupt - startInterrupt);
    if(free!=NULL){
        softBlockCount--;
        free->p_s.reg_v0 = status;
        //insertProcQ(&readyQueue, free);
    }
    returnControl();
}



HIDDEN void deviceHandler(int type){
    int i, device_nr;
    dtpreg_t* dev;
    memaddr* interrupt_bitmap = (memaddr*) CDEV_BITMAP_ADDR(type);
    
    device_nr = getDeviceNr(*interrupt_bitmap);
    dev = (dtpreg_t*) DEV_REG_ADDR(type, device_nr);

    i = INSTANCES_NUMBER * (type - 3) + device_nr;
    status = dev->status;
    dev->command = ACK;

    exitDeviceInterrupt(i);
}

HIDDEN void terminalHandler(){
    int i, read, device_nr;
    termreg_t* term;
    memaddr* interrupt_bitmap = (memaddr*) CDEV_BITMAP_ADDR(INT_TERMINAL);

    if ((device_nr = getDeviceNr(*interrupt_bitmap)) < 0) PANIC();
    else term = (termreg_t*) DEV_REG_ADDR(INT_TERMINAL, device_nr);

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
    
    exitDeviceInterrupt(i);
}

/**
 * @brief Handles a PLT interrupt.
 */
HIDDEN void PLTInterrupt(){
    setTIMER(LARGE_CONSTANT);
    currentProcess->p_s = *EXCEPTION_STATE;
    currentProcess->p_time += TIMESLICE;
    insertProcQ(&readyQueue, currentProcess);
    currentProcess = NULL;
    scheduler();
}

/**
 * @brief Handles a System Wide Interrupt.
 */
HIDDEN void SWITInterrupt(){
    LDIT(SWTIMER);
    pcb_t *removedProcess = NULL;

    while((removedProcess = removeBlocked(&swiSemaphore)) != NULL){
        // STCK(endInterrupt);
        // removedProcess->p_time += (endInterrupt - startInterrupt);
        insertProcQ(&readyQueue, removedProcess);
        // removedProcess = removeBlocked(&swiSemaphore);
        // if(removedProcess != NULL){
        //     removedProcess->p_time += (endInterrupt - startInterrupt);
        //     insertProcQ(&readyQueue, removedProcess);
        // }
    }
    softBlockCount += swiSemaphore;
    swiSemaphore = 0;

    returnControl();
}

void interruptHandler(state_t* excState){
    STCK(startInterrupt);
    unsigned int cause = (excState->cause);
    if(CAUSE_IP_GET(cause, INT_PLT)){
        PLTInterrupt();
    } else if(CAUSE_IP_GET(cause, INT_SWT)){
        SWITInterrupt();
    } else if(CAUSE_IP_GET(cause, INT_DISK)){
        deviceHandler(INT_DISK);
    } else if(CAUSE_IP_GET(cause, INT_TAPE)){
        deviceHandler(INT_TAPE);
    } else if(CAUSE_IP_GET(cause, INT_NETWORK)){
        deviceHandler(INT_NETWORK);
    } else if(CAUSE_IP_GET(cause, INT_PRINTER)){
        deviceHandler(INT_PRINTER);
    } else if(CAUSE_IP_GET(cause, INT_TERMINAL)){
        terminalHandler();
    } else {
        /* sollevato interrupt non riconosciuto */
        return; 
    }
}