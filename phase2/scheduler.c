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
        if(processCount>0 && softblockCount>0){    //Se ci son solo processi in attesa aspetta
            //Save the current state
            unsigned oldStatus = getSTATUS();
            //TODO: serve? setTIMER(PLTTIMER);
            //Disabilita gli interrupt
            setSTATUS(oldStatus & ~TEBITON | IMON | IECON);
            //Let's wait for an interrupt
            WAIT();
            //Restore the previous status
            setSTATUS(oldStatus);
        }
        if(processCount>0 && softblockCount==0){  //Se non ci son processi bloccati ma la queue è vuota PANICO
            prepanic();
            PANIC();
        }
    }
    //Se c'è un processo attivo lo rimetto in coda
    if(currentProcess != NULL){
        insertProcQ(&readyQueue, currentProcess);
        STCK(endTimeSlice);
        currentProcess->p_time += (endTimeSlice - startTimeSlice);
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