#include <pandos_types.h>
#include <initial.h>

int main(){
    //Inizializzare strutture dati
    initPcbs();
    initASL();

    //Parte sul passup Vector
    passupvector_t* passup;
    passup->tlb_refll_handler = (memaddr) uTLB_RefillHandler;
    passup->tlb_refill_stackPtr = (memaddr) KERNELSTACK;
    passup->exception_handler = (memaddr) //TODO: gestore delle eccezioni
    passup->exception_stackPtr = (memaddr) KERNELSTACK;
    PASSUPVECTOR = &passup;

    //Inizializzazione variabili
    processCount = 0;
    softblockCount = 0;
    readyQueue = mkEmptyProcQ();
    currentProcess = NULL;
    for(int i=0; i<DEVICENUMBER; i++){
        dev_sem[i] = 0;
    }

    //Setup del system-wide timer
    LDIT(TIMERVALUE);

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
