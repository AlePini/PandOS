#include <syscalls.h>
#include <umps3/umps/cp0.h>
#include <umps3/umps/libumps.h>
#include <asl.h>
#include <pcb.h>
#include <exceptions.h>
#include <initial.h>
#include <scheduler.h>


void createProcess(state_t* state, support_t* supportStruct){
    //Creates the new process and assigns the state and the support struct.
    pcb_t* newProcess = allocPcb();
    int feedback = -1;
    if(newProcess != NULL){
        processCount++;
        newProcess->p_s = *state;
        newProcess->p_supportStruct = supportStruct;
        insertChild(currentProcess, newProcess);
        insertProcQ(&readyQueue, newProcess);
        feedback = 0;
    }
    //Returns 0 or -1 based on what allocBCP returns (NULL or not).
    EXCEPTION_STATE->reg_v0 = feedback;
}

/**
 * @brief Recursively terminates a process and
 * its progeny.
 *
 * @param p Root process to kill.
 */
HIDDEN void terminateRecursive(pcb_t *p){

    pcb_t* removed;

	while ((removed = removeChild(p)) != NULL) {
        outProcQ(&readyQueue, removed);
		terminateRecursive(removed);
	}

    // A process is blocked on a device if the semaphore is
    // swiSemaphore or an element of semDevices.
    bool blockedDevice = ( p->p_semAdd >= (int*)semaphoreList
                        && p->p_semAdd < ((int *)semaphoreList + (sizeof(SEMAPHORE) * DEVICE_NUMBER)) )
                        || p->p_semAdd == (int*) &swiSemaphore;

    // If the process was not blocked on a device semaphore increment semadd by 1.
    pcb_t* removedProcess = outBlocked(p);

    if(!blockedDevice && removedProcess != NULL)
        (*(p->p_semAdd))++;

    freePcb(p);
    processCount--;
    return;
}

void terminateProcess(){
    //Removes current process from being his father's son.
    outChild(currentProcess);
    //Terminates the current and all his progeny.
	terminateRecursive(currentProcess);
	currentProcess = NULL;
	scheduler();
    return;
}

void passeren(int* semaddr){
    (*semaddr)--;
    if(*semaddr < 0){
        currentProcess->p_time += getTimeSlice();
        //Saves the current state of things and then block it, so when it restarts, it restarts from here.
        currentProcess->p_s = *EXCEPTION_STATE;
        insertBlocked(semaddr, currentProcess);
        currentProcess = NULL;
        scheduler();
    }
    return;
}

pcb_t* verhogen(int* semaddr){
    (*semaddr)++;
    pcb_t* tmp = NULL;
    if(*semaddr <= 0){
        tmp = removeBlocked(semaddr);
        if (tmp != NULL)
			insertProcQ(&readyQueue, tmp);
    }
    return tmp;
}

void waitIO(int intlNo, int dnum, int waitForTermRead){
    currentProcess->p_s = *EXCEPTION_STATE;
    softBlockCount++;

    //If it's not one of the expected device terminates the process.
    if(intlNo<3 || intlNo >7)
        terminateProcess();

    //Takes the line of interrupt (mapping them from 0 to 4) and multiply it by 8.
    //If it's a terminal line checks if it reads or transmits.
    int semaphoreIndex = INSTANCES_NUMBER*(intlNo+waitForTermRead-3) + dnum;

    passeren(&semaphoreList[semaphoreIndex]);
}

void getCpuTime(){
    unsigned time = currentProcess->p_time + getTimeSlice();
    EXCEPTION_STATE->reg_v0 = time;
    return;
}

void waitForClock(){
    softBlockCount++;
    passeren(&swiSemaphore);
    return;
}

void getSupportStruct(){
    EXCEPTION_STATE->reg_v0 = currentProcess->p_supportStruct;
    return;
}

void sysHandler(){

    //Reads the id of the syscall.
    unsigned sysdnum = EXCEPTION_STATE->reg_a0;

    //Recovery of the state of the exception.
    state_t state = *EXCEPTION_STATE;

    //Retrieve the informations of the syscall from the registers.
    unsigned arg1 = state.reg_a1;
    unsigned arg2 = state.reg_a2;
    unsigned arg3 = state.reg_a3;

    //Checks if syscall is valid.
    if(sysdnum >= 1 && sysdnum <= 8){

        //If it's beetween 1 and 8 it must be on kernel mode or sn exception is raised.
        if(CHECK_USERMODE(state.status)){
            EXCEPTION_STATE->cause &= ~GETEXECCODE;
            EXCEPTION_STATE->cause |= EXC_RI << CAUSESHIFT;
            generalTrapHandler();
        }
        else{
            switch(sysdnum){
                case CREATEPROCESS:
                    createProcess((state_t*) arg1, (support_t*) arg2);
                    break;
                case TERMPROCESS:
                    terminateProcess();
                    break;
                case PASSEREN:
                    passeren((int*) arg1);
                    break;
                case VERHOGEN:
                    verhogen((int*) arg1);
                    break;
                case IOWAIT:
                    waitIO(arg1, arg2, arg3);
                    break;
                case GETTIME:
                    getCpuTime();
                    break;
                case CLOCKWAIT:
                    waitForClock();
                    break;
                case GETSUPPORTPTR:
                    getSupportStruct();
                    break;
                default:
                    terminateProcess();
                    break;
            }
            if(currentProcess != NULL)
                LDST(EXCEPTION_STATE);
            else scheduler();
        }

    }
    //If the syscall wasn't from the previous 8 raises a trap exception.
    else generalTrapHandler();
}
