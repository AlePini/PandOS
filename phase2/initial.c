#include <pandos_types.h>
#include <initial.h>

//Dichiarazione variabili
int processCount;
int softblockCount;
pcb_t* readyQueue;
pcb_t* currentProcess;
int dev_sem[DEVICENUMBER];

int main(){

    //Inizializzazione variabili
    processCount = 0;
    softblockCount = 0;
    readyQueue = mkEmptyProcQ();
    currentProcess = NULL;
    for(int i=0; i<DEVICENUMBER; i++){
        dev_sem[i] = 0;
    }

    //Inizializzare strutture dati
    initPcbs();
    initASL();

    //Popolare il passup vector
    passupvector_t* passup;
    passup->tlb_refll_handler = (memaddr) uTLB_RefillHandler;
    passup->tlb_refill_stackPtr = (memaddr) KERNELSTACK;
    passup->exception_handler = (memaddr) //TODO: gestore delle eccezioni
    passup->exception_stackPtr = (memaddr) KERNELSTACK;
    PASSUPVECTOR = &passup;

    //Setup del system-wide timer
    LDIT(SWTIMERVALUE);

    //Parte sul primo processo
    pcb_t* firstProcess = allocPcb();
    //Status
    state_t p_s;
    STST(&p_s);

    //Setup dello status
    //In teoria USERPON non serve in quanto  facendo stst lui copia lo stato del processore che è gia kernel mode va testato
    /*USERPON Kernel mode ON */
    /*TEBITON Timer ON */
    /*IEPON Interrupt abilitati */
    /*IMON Attiva tutti gli interrupt */
    p_s.status |= ~USERPON | TEBITON | IEPON | IMON;

    //SP is set to RAMTOP
    RAMTOP(p_s.reg_sp);
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
