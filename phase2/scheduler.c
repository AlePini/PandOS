#include <scheduler.h>
#include <utils.h>

void scheduler(){

    if(emptyProcQ(getReadyQueue()) && processCount == 0){   //Se non ci sono più processi spegni
        HALT();
    }else if(emptyProcQ(getReadyQueue()) && processCount>0 && softblockCount>0){    //Se ci son solo processi in attesa aspetta
        WAIT();
    }else if (emptyProcQ(getReadyQueue()) && processCount>0 && softblockCount==0){  //Se non ci son processi bloccati ma la queue è vuota PANICO
        PANIC();
    }
    //Se c'è un processo attivo lo rimetto in coda
    //TODO: donno non ha messo sto if però penso serva. controllare poi
    if(currentProcess != NULL){
        insertProcQ(getReadyQueue(), currentProcess);
    }
    currentProcess = removeProcQ(getReadyQueue());
    setTIMER(PLTTIMER);
    LDST(&(currentProcess->p_s));
}