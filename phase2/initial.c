#include <umps3/umps/libumps.h>
#include "asl.h"
#include "pcb.h"
#include "exceptions.h"
#include "initial.h"
#include "scheduler.h"

//TODO: mettere HIDDEN tutto quello che viene usato solo nel suo file


//Dichiarazione variabili
unsigned int processCount;
unsigned int softBlockCount;
pcb_t* readyQueue;
pcb_t* currentProcess;
SEMAPHORE semaphoreList[DEVICE_NUMBER];
SEMAPHORE swiSemaphore;

//Prova a mettere la roba extern qui
extern void test();
extern void uTLB_RefillHandler();


int main(){

    //Inizializzare strutture dati livello 2
    initPcbs();
    initASL();

    //Popolare il passup vector
    passupvector_t* passup = (passupvector_t*)PASSUPVECTOR;
    passup->tlb_refill_handler = (memaddr) &uTLB_RefillHandler;
    passup->tlb_refill_stackPtr = (memaddr) KERNELSTACK;
    passup->exception_handler = (memaddr) exceptionHandler;
    passup->exception_stackPtr = (memaddr) KERNELSTACK;


    // //Inizializzare le variabili globali
    readyQueue = mkEmptyProcQ();
    currentProcess = NULL;
    processCount = 0;
    softBlockCount= 0;
    swiSemaphore = 0;
    for (int i=0; i<DEVICE_NUMBER; i++){
        semaphoreList[i] = 0;
    }


    //Setup del system-wide timer
    LDIT(SWTIMER);

    //Parte sul primo processo
    pcb_t* firstProcess = allocPcb();
    processCount++;

    //Setup dello status
    firstProcess->p_s.status = ALLOFF | TEBITON | IEPON | IMON;
    //SP is set to RAMTOP

    RAMTOP(firstProcess->p_s.reg_sp);
    //Program Counter set to test address
    firstProcess->p_s.pc_epc = (memaddr) test;
    firstProcess->p_s.reg_t9 = (memaddr) test;
    insertProcQ(&readyQueue, firstProcess);

    //Chiamo lo scheduler
    scheduler();
    
    return 0;
}
