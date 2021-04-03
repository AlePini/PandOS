#include <syscalls.h>
#include <scheduler.h>
#include <utils.h>
#include <exceptions.h>
#include <initial.h>


void sysHandler(){

    //Leggo l'id della syscall
    unsigned sysdnum = EXCEPTION_STATE -> reg_a0;

    //Recupero lo stato dell'eccezione
    state_t state = *EXCEPTION_STATE;

    //Recupero le informazioni sulla syscall dai registri
    unsigned arg1 = state.reg_a1;
    unsigned arg2 = state.reg_a2;
    unsigned arg3 = state.reg_a3;

    //Controllo la syscall sia valida 
    if(sysdnum >= 1 && sysdnum <= 8){
        
        //Se è fra 1 e 8 devo essere in kernel mode altrimenti sollevo un'eccezione
        if(CHECK_USERMODE(state.status)){
            setCAUSE(EXC_RI<<CAUSESHIFT);
            exceptionHandler();
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
                    PANIC();
                    break;
            }
            LDST(EXCEPTION_STATE);
        }

    }//Se la syscall non era fra le 8 possibili sollevo una Trap Exception
    else exceptionHandler(GENERAL);
}


void createProcess(state_t* state, support_t* supportStruct){
    //Creo il nuovo processo e gli assegno lo stato e la support struct.
    pcb_t* newProcess = allocPcb();
    int feedback = -1;
    if(newProcess != NULL){
        newProcess->p_s = *state;
        newProcess->p_supportStruct = supportStruct;
        insertChild(currentProcess, newProcess);
        insertProcQ(&readyQueue, newProcess);
        feedback = 0;
    }
    //Ritorna 1 o -1 in base a se allocPCB ritorna NULL o meno.
    EXCEPTION_STATE->reg_v0 = feedback;
}

void terminateProcess(){
    //Rimuovo il current process dall'essere figlio di suo padrew
    outChild(currentProcess);
    //Termino il current e tutta la sua progenie.
	termProcessRecursive(currentProcess);
	currentProcess = NULL;
	scheduler();
    return;
}

/**
 * @brief Recursively terminates a process and
 * its progeny.
 * 
 * @param p Root process to kill.
 */
HIDDEN void terminateRecursive(pcb_t *p) {
    pcb_t* child;
    
    
    // REcursively kills all the progeny
	while ((child = removeChild(p)) != NULL) {
		termProcessRecursive(child);
	}

    // Handle the process itself
    if(p->p_semAdd != NULL){
        // A process is blocked on a device if the semaphore is
        // swiSemaphore or an element of semDevices
        bool blockedDevice =
            (p->p_semAdd >= (int*)semaphoreList &&
            p->p_semAdd < ((int *)semaphoreList + (sizeof(SEMAPHORE) * DEVICE_TYPES * INSTANCES_NUMBER)))
            || (p->p_semAdd == (int*) &swiSemaphore);
        
        // If the process is blocked on a user semaphore, remove it
        pcb_t* removedProcess = outBlocked(p);

        // For device processes, 
        if (!blockedDevice && removedProcess != NULL) {
            (*(p->p_semAdd))++;
        }
    }
    else outProcQ(&readyQueue, p);

    processCount--;
    freePcb(p);
    return;
}

void passeren(int* semaddr){
    (*semaddr)--;
    if(*semaddr < 0){
        STCK(endTimeSlice);
        currentProcess->p_time+=(endTimeSlice-startTimeSlice);
        //Salvi lo stato attuale delle cose per poi bloccarlo, così che quando ricomincia ricomincia da qui
        currentProcess->p_s = *EXCEPTION_STATE;
        insertBlocked(semaddr, currentProcess);
        currentProcess=NULL;
        scheduler();
    }
    return;
}

pcb_t* verhogen(int* semaddr){
    (*semaddr)++;
    if(*semaddr <= 0){
        pcb_t* tmp = removeBlocked(semaddr);
        insertProcQ(&readyQueue,tmp);
        return tmp;
    }
    return NULL;
}

void waitIO(int intlNo, int  dnum, int waitForTermRead){
    //Se non è uno dei device previsti termino il processo.
    if(intlNo<3 || intlNo >7) terminateProcess();
    //Prendo la linea di interrupt(rimappandole da 0 a 4) e la moltiplico per 8. Se è un terminal line controllo se leggo o trasmetto.
    int semaphoreIndex = INSTANCES_NUMBER*(intlNo+waitForTermRead-3) + dnum;
    softblockCount++;
    passeren(&semaphoreList[semaphoreIndex]);
}

void getCpuTime(){
    STCK(endTimeSlice);
    unsigned time = currentProcess->p_time + (endTimeSlice-startTimeSlice);
    EXCEPTION_STATE->reg_v0 = time;
    return;
}

void waitForClock(){
    softblockCount++;
    passeren(&swiSemaphore);
    return;
}

void getSupportStruct(){
    EXCEPTION_STATE->reg_v0 = currentProcess->p_supportStruct;
    return;
}