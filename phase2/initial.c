#include <initial.h>
#include <scheduler.h>
#include <exceptions.h>
#include <umps3/umps/libumps.h>
//TODO: mettere HIDDEN tutto quello che viene usato solo nel suo file

//Dichiarazione variabili
int processCount;
int softblockCount;
pcb_t* readyQueue;
pcb_t* currentProcess;

int semDisk[INSTANCES_NUMBER];
int semFlash[INSTANCES_NUMBER];
int semNetwork[INSTANCES_NUMBER];
int semPrinter[INSTANCES_NUMBER];
int semTerminalTrans[INSTANCES_NUMBER];
int semTerminalRecv[INSTANCES_NUMBER];
int semIntTimer;

//Prova a mettere la roba extern qui
extern void test();
extern void uTLB_RefillHandler();
extern void exceptionHandler();

void prova(){
    return;
}

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
    //processCount, softBlockCount and device semaphores are automatically initialized at compile time


    //Setup del system-wide timer
    LDIT(100000UL);
    //LDIT(SWTIMERVALUE);

    //Parte sul primo processo
    pcb_t* firstProcess = allocPcb();
    processCount++;

    //Setup dello status
    /*TEBITON Timer ON */
    /*IEPON Interrupt abilitati */
    /*IMON Attiva tutti gli interrupt */
    firstProcess->p_s.status = ALLOFF | TEBITON | IEPON | IMON;
    setSTATUS(firstProcess -> p_s.status);
    //copyState(&firstProcess, (&currentProcess->p_s));

    //SP is set to RAMTOP
    RAMTOP(firstProcess->p_s.reg_sp);
    //Program Counter set to test address
    firstProcess->p_s.pc_epc = (memaddr) &test;
    firstProcess->p_s.reg_t9 = (memaddr) &test;
    insertProcQ(&readyQueue, firstProcess);
    prova();
    LDST(&(currentProcess->p_s));

    //Scheduler
    scheduler();

    return 0;
}
