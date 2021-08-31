#include <syscalls.h>
#include <scheduler.h>
#include <initial.h>
#include <pandos_const.h>
#include <pandos_types.h>
#include <asl.h>
#include <pcb.h>
#include <umps3/umps/cp0.h>


extern cpu_t startTimeSlice;  //servono per misurare l'intervallo di tempo
extern cpu_t endTimeSlice;
extern memaddr* getDevRegAddr(int int_line, int dev_n);


void copyState(state_t *source, state_t *dest)
{   
    dest->cause = source->cause;
    dest->entry_hi = source->entry_hi;
    dest->hi = source->hi;
    dest->lo = source->lo;
    dest->pc_epc = source->pc_epc;
    dest->status = source->status;
    for (unsigned int i = 0; i < STATE_GPR_LEN; i++)
    {
        dest->gpr[i] = source->gpr[i];
    }
}

/**
 * @brief Funzione che calcola l'indice del semaforo del device nell'array
 * 
 * @param line 
 * @param device 
 * @param read 
 * @return int 
 */
int getDeviceSemaphoreIndex(int line, int device, int read)
{
    return ((line - 3) * 8) + (line == 7 ? (read * 8) + device : device);
}

void createProcess(state_t* state, support_t* supportStruct)
{
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
 * @brief Funzione di supporto per terinare la progenie di current
 * 
 * @param p 
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
}

void terminateProcess(){
    //Removes current process from being his father's son.
    outChild(currentProcess);
    //Terminates the current and all his progeny.
	terminateRecursive(currentProcess);
	currentProcess = NULL;
	scheduler();
}

void passeren(state_t *statep)
{
    int *semadd = (int*) statep->reg_a1;    //prendiamo l'indirizzo del semaforo
    (*semadd)--;
    if(*semadd < 0){
        //Saves the current state of things and then block it, so when it restarts, it restarts from here.
        currentProcess->p_s = *EXCEPTION_STATE;
        insertBlocked(semadd, currentProcess);
        currentProcess = NULL;
        scheduler();
    }
}

pcb_t* verhogen(state_t *statep)
{
    int *semadd = (int*) statep->reg_a1;    //prendiamo l'indirizzo del semaforo
    (*semadd)++;
    pcb_t* tmp = NULL;
    if(*semadd <= 0){
        tmp = removeBlocked(semadd);
        if (tmp != NULL)
			insertProcQ(&readyQueue, tmp);
    }
    // TODO controla return and LDST
    //LDST(statep);
}

void waitForIO(state_t *statep)
{
    int line = statep->reg_a1;  //preleviamo la line dal registro a1
    int device = statep->reg_a2; //preleviamo il device dal registro a2
    int read = statep->reg_a3;  //preleviamo se e' read da terminale da a3
    
    copyState(statep, &(currentProcess->p_s));   //copiamo lo stato
    softBlockCount++; //aumentiamo i soft blocked

    if (line < DISKINT || line > TERMINT)   //fuori dal range delle linee
        terminateProcess(); //terminiamo il processo

    int index = getDeviceSemaphoreIndex(line, device, read);  //calcolo indice semaforo
    int *sem = &(semaphoreList[index]);   //prendiamo l'indirizzo di tale semaforo
    (*sem)--;   //decrementiamo il semaforo
    insertBlocked(sem, currentProcess);  //blocchiamo il processo
    scheduler();    //scheduler
}

void getCpuTime(state_t *statep) //TODO da controllare se è necessario aggiornare qui p_time
{
    cpu_t currTime;
    STCK(currTime); //tempo attuale
    currentProcess->p_time += (currTime - startTimeSlice); //aggiorno il tempo
    statep->reg_v0 = currentProcess->p_time; //preparo il regitro di ritorno
    STCK(startTimeSlice); //faccio ripartire il "cronometro"
}

void waitForClock(state_t *statep)
{
    softBlockCount++;
    passeren(&swiSemaphore);

}

void getSupportData(state_t *statep)
{
    EXCEPTION_STATE->reg_v0 = currentProcess->p_supportStruct;
}

void sysHandler(){

    //Reads the id of the syscall.
    volatile unsigned sysdnum = EXCEPTION_STATE->reg_a0;

    volatile unsigned arg1 = EXCEPTION_STATE->reg_a1;
    volatile unsigned arg2 = EXCEPTION_STATE->reg_a2;
    volatile unsigned arg3 = EXCEPTION_STATE->reg_a3;

    //Checks if syscall is valid.
    if(sysdnum >= 1 && sysdnum <= 8){

        //If it's beetween 1 and 8 it must be on kernel mode or sn exception is raised.
        if(CHECK_USERMODE(EXCEPTION_STATE->status)){
            EXCEPTION_STATE->cause &= ~GETEXECCODE;
            EXCEPTION_STATE->cause |= EXC_RI << CAUSESHIFT;
            generalTrapHandler();
        }
        else{
            EXCEPTION_STATE->pc_epc += WORDLEN;
            switch(sysdnum){
                case CREATEPROCESS:
                    createProcess(arg1, arg2);
                    break;
                case TERMPROCESS:
                    terminateProcess();
                    break;
                case PASSEREN:
                    passeren(EXCEPTION_STATE);
                    break;
                case VERHOGEN:
                    verhogen(EXCEPTION_STATE);
                    break;
                case IOWAIT:
                    waitForIO(EXCEPTION_STATE);
                    break;
                case GETTIME:
                    getCpuTime(EXCEPTION_STATE);
                    break;
                case CLOCKWAIT:
                    waitForClock(EXCEPTION_STATE);
                    break;
                case GETSUPPORTPTR:
                    getSupportData(EXCEPTION_STATE);
                    break;
                    /*syscall sconosciuta*/
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