#include <scheduler.h>
#include <umps3/umps/libumps.h>
#include <pcb.h>
#include <initial.h>


cpu_t startTimeSlice,endTimeSlice = 0; 

void scheduler(){
    //If there's an active process puts it in the queue.
    if(currentProcess != NULL){
        //insertProcQ(&readyQueue, currentProcess);
        STCK(endTimeSlice);
        currentProcess->p_time += (endTimeSlice - startTimeSlice);
    }
    if(emptyProcQ(readyQueue)){
        currentProcess = NULL;
        if(processCount == 0)   //If there's no more process shuts down.
            HALT();
        if(processCount>0 && softBlockCount>0){    //Waits if there's only waiting processes.
            //Saves the current state.
            unsigned oldStatus = getSTATUS();
            setTIMER(PLTBLOCK);
            //Enables the interrupts.
            setSTATUS(ALLOFF | IMON | IECON);
            //Waits for an interrupt.
            WAIT();
            //Restores the previous status.
            setSTATUS(oldStatus);
        }
        if(processCount>0 && softBlockCount==0) //If there's no blocked process but the queue is empty PANIC.
            PANIC();
    } else {
        //Replace the current process.
        currentProcess = removeProcQ(&readyQueue);
        //Reset the timer
        setTIMER(PLTTIMER);
        //Starts to count the time of this process.
        STCK(startTimeSlice);
        //Loads the new state.
        LDST(&(currentProcess->p_s));
    }
}