#include <scheduler.h>
#include <initial.h>
#include <stdlib.h>
unsigned startTimeSlice,endTimeSlice;

//Dichiarazione variabili
extern int processCount;
extern int softblockCount;
extern pcb_t* readyQueue;
extern pcb_t* currentProcess;

extern SEMAPHORE semaphoreList[];
extern SEMAPHORE semIntTimer;

void scheduler(){
    if(emptyProcQ(readyQueue)){
        if(processCount == 0){   //Se non ci sono più processi spegni
            HALT();
        }
        if(processCount>0 && softblockCount>0){    //Se ci son solo processi in attesa aspetta
            unsigned oldStatus = getSTATUS();
            hello();
            setSTATUS(oldStatus & ~TEBITON | IMON | IECON);
            WAIT();
            setSTATUS(oldStatus);
        }
        if(processCount>0 && softblockCount==0){  //Se non ci son processi bloccati ma la queue è vuota PANICO
            prepanic();
            PANIC();
        }
    }
    //Se c'è un processo attivo lo rimetto in coda
    //TODO: donno non ha messo sto if però penso serva. controllare poi
    if(currentProcess != NULL){
        insertProcQ(&readyQueue, currentProcess);
        STCK(endTimeSlice);
        currentProcess->p_time += (endTimeSlice - startTimeSlice);
    }
    currentProcess = removeProcQ(&readyQueue);
    setTIMER(PLTTIMER);
    STCK(startTimeSlice);
    LDST(&(currentProcess->p_s));
}

void prepanic(){
    return;
}

void hello(){
    return;   
}