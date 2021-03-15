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
    passup->tlb refll handler = (memaddr) uTLB RefillHandler;
    passup->tlb_refill_stackPtr = (memaddr) 0x20001000;
    passup->exception handler = (memaddr) //TODO: gestore delle eccezioni
    passup->exception_stackPtr = (memaddr) 0x20001000;
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
    pcb_t* process = createFirstProcess((memaddr)test); //TODO: da fare nel utils.c
    processCount++;
    insertProcQ(&readyQueue, process);

    //Scheduler
    scheduler();

    //Non so se ci vuole il return 0
    return 0;
}
