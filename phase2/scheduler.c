#include <scheduler.h>
#include <initial.h>
#include <stdlib.h>
unsigned startTimeSlice,endTimeSlice;
// extern int processCount;
// extern int softblockCount;
// extern pcb_t* readyQueue;
// extern pcb_t* currentProcess=NULL;

void scheduler(){
    if(emptyProcQ(readyQueue)){
        if(processCount == 0){   //Se non ci sono più processi spegni
            HALT();
        }
        if(processCount>0 && softblockCount>0){    //Se ci son solo processi in attesa aspetta
            unsigned oldStatus = getSTATUS();
            setSTATUS(oldStatus & ~TEBITON | IMON | IECON);
            WAIT();
            setSTATUS(oldStatus);
        }
        if(processCount>0 && softblockCount==0){  //Se non ci son processi bloccati ma la queue è vuota PANICO
            PANIC();
        }
    }
    //Se c'è un processo attivo lo rimetto in coda
    //TODO: donno non ha messo sto if però penso serva. controllare poi
    if(currentProcess != NULL){
        prova(currentProcess);
        insertProcQ(&readyQueue, currentProcess);
    }
    prova();
    currentProcess = removeProcQ(&readyQueue);
    setTIMER(PLTTIMER);
    STCK(startTimeSlice);
    LDST(&(currentProcess->p_s));
}

void prova(){
    pcb_t* boh = NULL;
    if(boh == currentProcess)
        prova2();
    return;
}

void prova2(){
    return;
}