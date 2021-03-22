#include <scheduler.h>
#include <utils.h>

unsigned insertTime,removeTime;

void scheduler(){
    //TODO: per fare halt e panic devo essere in kernel mode
    if(emptyProcQ(getReadyQueue()) && processCount == 0){   //Se non ci sono più processi spegni
        if(*((int*)STATUS_KUp) == 1){
            setCause(EXC_BP <<CAUSESHIFT);
            exceptionHandler(exceptionType());
        }
        else HALT();
    }else if(emptyProcQ(getReadyQueue()) && processCount>0 && softblockCount>0){    //Se ci son solo processi in attesa aspetta
        if((*((int*)STATUS_KUp) == 1) || (*((int*) STATUS_CU0) ==0)){
            setCause(EXC_RI<<CAUSESHIFT);
            exceptionHandler(exceptionType());
        }
        else{
            currentProcess->p_s.status |= IECON;
            *((int*)STATUS_TE) = 0;
            WAIT();
        }
    }else if (emptyProcQ(getReadyQueue()) && processCount>0 && softblockCount==0){  //Se non ci son processi bloccati ma la queue è vuota PANICO
        if(*((int*)STATUS_KUp) == 1){
            setCause(EXC_BP <<CAUSESHIFT);
            exceptionHandler(exceptionType());
        }
        else PANIC();
    }
    //Se c'è un processo attivo lo rimetto in coda
    //TODO: donno non ha messo sto if però penso serva. controllare poi
    if(currentProcess != NULL){
        insertProcQ(getReadyQueue(), currentProcess);
    }
    currentProcess = removeProcQ(getQueue());
    setTIMER(PLTTIMER);
    LDST(&(currentProcess->p_s));
}