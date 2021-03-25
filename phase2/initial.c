#include <initial.h>
//TODO: mettere HIDDEN tutto quello che viene usato solo nel suo file

//Dichiarazione variabili
int processCount;
int softblockCount;
pcb_t* readyQueue;
pcb_t* currentProcess;

//TODO: Da rifare come array
SEMAPHORE semaphoreList[DEVICE_NUMBER];
SEMAPHORE semIntTimer;
// int semDisk[INSTANCES_NUMBER];
// int semFlash[INSTANCES_NUMBER];
// int semNetwork[INSTANCES_NUMBER];
// int semPrinter[INSTANCES_NUMBER];
// int semTerminalTrans[INSTANCES_NUMBER];
// int semTerminalRecv[INSTANCES_NUMBER];


//TODO: capire dove metterle

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


    //Inizializzare le variabili globali
    readyQueue = mkEmptyProcQ();
    currentProcess = NULL;
    //processCount, softBlockCount and device semaphoressi inizializzano da soli ai valori di default


    //Setup del system-wide timer
    //TODO: capire quale dei due sia giusto
    LDIT(100000UL);
    //LDIT(SWTIMERVALUE);

    //Parte sul primo processo
    pcb_t* firstProcess = allocPcb();
    processCount++;

    //Setup dello status
    firstProcess->p_s.status = ALLOFF | TEBITON | IEPON | IMON;

    //SP is set to RAMTOP
    RAMTOP(firstProcess->p_s.reg_sp);
    //Program Counter set to test address
    firstProcess->p_s.pc_epc = (memaddr) &test;
    firstProcess->p_s.reg_t9 = (memaddr) &test;
    insertProcQ(&readyQueue, firstProcess);

    //Scheduler
    scheduler();

    return 0;
}
