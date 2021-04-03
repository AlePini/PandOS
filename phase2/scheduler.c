#include <umps3/umps/libumps.h>
#include <pcb.h>
#include <scheduler.h>
#include <initial.h>


cpu_t startTimeSlice,endTimeSlice = 0; 

void scheduler(){
    if(emptyProcQ(readyQueue)){
        if(processCount == 0){   //Se non ci sono più processi spegni
            HALT();
        }
        if(processCount>0 && softBlockCount>0){    //Se ci son solo processi in attesa aspetta
            //Save the current state
            unsigned oldStatus = getSTATUS();
            setTIMER(LARGE_CONSTANT);
            //Disabilita gli interrupt
            setSTATUS(oldStatus | IMON | IECON);
            //Let's wait for an interrupt
            WAIT();
            //Restore the previous status
            setSTATUS(oldStatus);
        }
        if(processCount>0 && softBlockCount==0){  //Se non ci son processi bloccati ma la queue è vuota PANICO
            if(softBlockCount == 0) cprova();
            PANIC();
        }
    }
    //Se c'è un processo attivo lo rimetto in coda
    if(currentProcess != NULL){
        insertProcQ(&readyQueue, currentProcess);
        currentProcess->p_time += getTimeSlice();
    }
    //Sosituisco il current process
    currentProcess = removeProcQ(&readyQueue);
    //Resetto il timer
    setTIMER(PLTTIMER);
    //Inizio a contare il tempo di questo processo
    STCK(startTimeSlice);
    //Carico lo stato nuovo
    LDST(&(currentProcess->p_s));
}

cpu_t getTimeSlice(){
    STCK(endTimeSlice);
    return endTimeSlice - startTimeSlice;
}

void cprova(){
    return;
}