#include <umps3/umps/libumps.h>
#include "asl.h"
#include "pcb.h"
#include "exceptions.h"
#include "initial.h"
#include "scheduler.h"


//Variables declaration.
unsigned int processCount;
unsigned int softBlockCount;
pcb_t* readyQueue;
pcb_t* currentProcess;
SEMAPHORE semaphoreList[DEVICE_NUMBER];
SEMAPHORE swiSemaphore;

extern void test();
extern void uTLB_RefillHandler();


int main(){

    //Initialize data structure level 2.
    initPcbs();
    initASL();

    //Populates the passup vector.
    passupvector_t* passup = (passupvector_t*)PASSUPVECTOR;
    passup->tlb_refill_handler = (memaddr) &uTLB_RefillHandler;
    passup->tlb_refill_stackPtr = (memaddr) KERNELSTACK;
    passup->exception_handler = (memaddr) exceptionHandler;
    passup->exception_stackPtr = (memaddr) KERNELSTACK;


    //Initializes global variables.
    readyQueue = mkEmptyProcQ();
    currentProcess = NULL;
    processCount = 0;
    softBlockCount= 0;
    swiSemaphore = 0;
    for (int i=0; i<DEVICE_NUMBER; i++){
        semaphoreList[i] = 0;
    }


    //Setup del system-wide timer.
    LDIT(SWTIMER);

    //Creation of first process.
    pcb_t* firstProcess = allocPcb();
    processCount++;

    //Setup of status.
    firstProcess->p_s.status = ALLOFF | TEBITON | IEPON | IMON;
    //SP is set to RAMTOP.

    RAMTOP(firstProcess->p_s.reg_sp);
    //Program Counter set to test address.
    firstProcess->p_s.pc_epc = (memaddr) test;
    firstProcess->p_s.reg_t9 = (memaddr) test;
    insertProcQ(&readyQueue, firstProcess);

    //Call of the scheduler.
    scheduler();
    
    return 0;
}
