#include <pandos_types.h>
#include <initial.h>

extern void test();

HIDDEN int processCount;
HIDDEN int softblockCount;
HIDDEN pcb_t* readyQueue;
HIDDEN pcb_t* currentProcess;
HIDDEN int dev_sem[DEVICECNT];



int main(){
    //Inizializzare strutture dati
    initPcbs();
    initASL();

    //Parte sul passup Vector
    passupvector_t* passup;
    passup->tlb_refll_handler = (memaddr) uTLB RefillHandler;
    passup->tlb_refill_stackPtr = (memaddr) KERNELSTACK;
    passup->exception handler = (memaddr) //TODO: gestore delle eccezioni
    passup->exception_stackPtr = (memaddr) KERNELSTACK;
    PASSUPVECTOR = &passup;

    //Inizializzazione variabili
    processCount = 0;
    softblockCount = 0;
    readyQueue = mkEmptyProcQ();
    currentProcess = NULL;
    for(int i=0; i<DEVICECNT; i++){
        dev_sem[i] = 0;
    }

    //Setup del system-wide timer
    unsigned int T = TIMERLENGTH/TIMESCALEADDR;
    LDIT(T);

    //Parte sul primo processo
    pcb_t* firstProcess = allocPcb();
    //Status
    state_t p_s;
    STST(&p_s);
    p_s.status |= ~USERPON; /* Kernel mode ON */
    p_s.status |= TEBITON; /* Timer ON */
    p_s.status |= IEPON; /* Interrupt abilitati */
    p_s.status |= IMON; /* Attiva tutti gli interrupt */

    RAMTOP(firstProcess->p_s.reg_sp);   //SP is set to RAMTOP
    //Program Counter set to test address
    p_s.pc_epc = (memaddr) &test;
    p_s.reg_t9 = (memaddr) &test;
    process->p_s = p_s;
    //Incremented program counter and inserted the process in the readyqueue
    processCount++;
    insertProcQ(&readyQueue, firstProcess);

    //Scheduler
    scheduler();

    //Non so se ci vuole il return 0
    return 0;
}
