#include <syscalls.h>
#include <scheduler.h>
#include <utils.h>
#include <exceptions.h>
//Dichiarazione variabili
extern int processCount;
extern int softblockCount;
extern pcb_t* readyQueue;
extern pcb_t* currentProcess;

extern SEMAPHORE semaphoreList[];
extern SEMAPHORE semIntTimer;

extern unsigned endTimeSlice;
extern unsigned startTimeSlice;

void sysHandler(){
    //Leggo l'id della syscall
    unsigned sysdnum = EXCTYPE -> reg_a0;
    if(sysdnum >= 1 && sysdnum <= 8){
        //currentProcess->p_s = *EXCTYPE;
        state_t state = *EXCTYPE;
        //Se è fra 1 e 8 devo essere in kernel mode altrimenti eccezione
        if(CHECK_USERMODE(state.status)){
            setCAUSE(EXC_RI<<CAUSESHIFT);
            exceptionHandler();
        }
        else{
            //Se va tutto bene controllo che syscall sia
            unsigned arg1 = state.reg_a1;
            unsigned arg2 = state.reg_a2;
            unsigned arg3 = state.reg_a3;

            switch(sysdnum){
                case CREATEPROCESS:
                    createProcess((state_t*) arg1, (support_t*) arg2);
                    break;
                case TERMPROCESS:
                //Scheduler
                    terminateProcess();
                    break;
                case PASSEREN:
                //In alcuni casi scheduler
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
            //TODO: PENSO NON VADA BENE
            LDST(EXCTYPE);
        }
    }else{
        exceptionHandler(GENERAL);
    }
}

void createProcess(state_t* arg1, support_t* arg2){
    pcb_t* newProcess = allocPcb();
    int feedback = -1;
    if(newProcess != NULL){
        newProcess->p_s = *arg1;
        newProcess->p_supportStruct = arg2;
        newProcess->p_child = NULL;
        insertChild(currentProcess, newProcess);
        insertProcQ(&readyQueue, newProcess);
        feedback = 1;
    }
    // newProcess->p_time = 0;
    // newProcess->p_semAdd = NULL;
    EXCTYPE->reg_v0 = feedback;
}

void terminateProcess(){
    sgrodolox2();
    if(emptyChild(currentProcess)){
        outChild(currentProcess);
        freePcb(currentProcess);
        processCount--;
    }else{
        terminateRecursive(currentProcess);
    }
    currentProcess = NULL;
    scheduler();
    return;
}

//TODO: riscrivere diversa da donno
void terminateRecursive(pcb_t *p) {
    sgrodolox();
    pcb_t* child;
    while((child = removeChild(p)) != NULL){
        terminateRecursive(removeChild(p));
    }
    sgrodolox2();
    // Handle the process itself
    if(p->p_semAdd != NULL){
        // A process is blocked on a device if the semaphore is
        // semIntTimer or an element of semDevices
        bool blockedDevice =
            (p->p_semAdd >= (int*)semaphoreList &&
            p->p_semAdd < ((int *)semaphoreList + (sizeof(SEMAPHORE) * DEVICE_TYPES * INSTANCES_NUMBER)))
            || (p->p_semAdd == (int*)semIntTimer);
        
        // If the process is blocked on a user semaphore, remove it
        pcb_t* removedProcess = outBlocked(p);

        // For device processes, 
        if (!blockedDevice && removedProcess != NULL) {
            (*(p->p_semAdd))++;
        }
    }
    else if(p == currentProcess){
        outChild(currentProcess);
    }
    else{
        outProcQ(&readyQueue, p);
    }

    processCount--;
    freePcb(p);
    return;
}

void passeren(int* semaddr){
    (*semaddr)--;
    if(*semaddr < 0){
        STCK(endTimeSlice);
        currentProcess->p_time+=(endTimeSlice-startTimeSlice);
        //TODO:Ti salvi lo stato attuale delle cose per poi bloccarlo, così che quando ripiglia ricomincia da qui
        currentProcess->p_s = *EXCTYPE;
        insertBlocked(semaddr, currentProcess);
        currentProcess=NULL;
        scheduler();
    }
    LDST(EXCTYPE);
    return;
}

pcb_t* verhogen(int* semaddr){
    (*semaddr)++;
    if(*semaddr <= 0){
        provax();
        pcb_t* tmp = removeBlocked(semaddr);
        insertProcQ(&readyQueue,tmp);
        return tmp;
    }
    return NULL;
}

void waitIO(int intlNo, int  dnum, int waitForTermRead){
    //TODO: potrei far una matrice di semafori che è stra easy
    if(intlNo<3 || intlNo >7) terminateProcess();
    //Prendo la linea di interrupt(rimappandole da 0 a 4) e la moltiplico per 8. Se è un terminal line controllo se leggo e trasmetto
    int semaphoreIndex = INSTANCES_NUMBER*(intlNo+waitForTermRead-3) + dnum;
    provax();
    softblockCount++;
    passeren(&semaphoreList[semaphoreIndex]);
}

void getCpuTime(){
    STCK(endTimeSlice);
    unsigned time = currentProcess->p_time + (endTimeSlice-startTimeSlice);
    EXCTYPE->reg_v0 = time;
    return;
}

void waitForClock(){
    softblockCount++;
    passeren(&semIntTimer);
    return;
}

void getSupportStruct(){
    //TODO: va bene?
    EXCTYPE->reg_v0 = currentProcess->p_supportStruct;
    return;
}

void provax(){
    return;
}

void provax2(){
    return;
}

void sgrodolox(){
    return;
}

void sgrodolox2(){
    return;
}