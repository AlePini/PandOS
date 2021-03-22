#include <initial.h>
#include <scheduler.h>

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

int main(){

    //Inizializzazione variabili
    processCount = 0;
    softblockCount = 0;
    readyQueue = mkEmptyProcQ();
    currentProcess = NULL;
    for(int i=0; i<i; i++){
        semDisk[i]=0;
        semFlash[i]=0;
        semNetwork[i]=0;
        semPrinter[i]=0;
        semTerminalTrans[i]=0;
        semTerminalRecv[i]=0;
    }
    semIntTimer=0;

    //Inizializzare strutture dati
    initPcbs();
    initASL();

    //Popolare il passup vector
    passupvector_t* passup;
    passup->tlb_refill_handler = (memaddr) uTLB_RefillHandler;
    passup->tlb_refill_stackPtr = (memaddr) KERNELSTACK;
    passup->exception_handler = (memaddr) exceptionHandler;
    passup->exception_stackPtr = (memaddr) KERNELSTACK;

    //Setup del system-wide timer
    LDIT(SWTIMERVALUE);

    //Parte sul primo processo
    pcb_t* firstProcess = allocPcb();
    // TODO : in teoria non serve
    STST(&firstProcess->p_s);

    //Setup dello status
    //In teoria USERPON non serve in quanto  facendo stst lui copia lo stato del processore che è gia kernel mode va testato
    /*USERPON Kernel mode ON */
    /*TEBITON Timer ON */
    /*IEPON Interrupt abilitati */
    /*IMON Attiva tutti gli interrupt */
    
    firstProcess->p_s.status |= ~USERPON | TEBITON | IEPON | IMON;

    //SP is set to RAMTOP
    RAMTOP(firstProcess->p_s.reg_sp);
    //Program Counter set to test address
    firstProcess->p_s.pc_epc = (memaddr) &test;
    firstProcess->p_s.reg_t9 = (memaddr) &test;
    //Incremented program counter and inserted the process in the readyqueue
    processCount++;
    insertProcQ(&readyQueue, firstProcess);

    //Scheduler
    scheduler();

    //Non so se ci vuole il return 0
    return 0;
}
