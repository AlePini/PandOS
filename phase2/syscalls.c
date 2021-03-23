#include <syscall.h>
#include <scheduler.h>
#include <utils.h>
#include <exception.h>


void sysHandler(){
    //Leggo l'id della syscall
    unsigned sysdnum = currentProcess->p_s.reg_a0;
    if(sysdnum >=1 && sysdnum<=8){
        //Se è fra 1 e 8 devo essere in kernel mode altrimenti eccezione
        currentProcess->p_s = EXCTYPE;
        if(CHECK_USERMODE(currentProcess->p_s.status)){
            setCause(EXC_RI<<CAUSESHIFT);
            exceptionHandler();
        }
        else{
            //Se va tutto bene controllo che syscall sia
            unsigned arg1 = currentProcess->p_s.reg_a1;
            unsigned arg2 = currentProcess->p_s.reg_a2;
            unsigned arg3 = currentProcess->p_s.reg_a3;
            currentProcess->p_s.pc_epc += 4;
            switch(sysdnum){
                case CREATEPROCESS:
                    createProcess((state_t*) arg1, (support_t*) arg2);
                    break;
                case TERMPROCESS:
                //Scheduler
                    terminateProcess(void);
                    break;
                case PASSEREN:
                //In alcuni casi scheduler
                    passeren((int*) arg1);
                    break;
                case VERHOGEN:
                    verhogen((int*) arg1);
                    break;
                case IOWAIT:
                    waitIO(int arg1, int arg2, int arg3);
                    break;
                case GETTIME:
                    getCpuTime(void);
                    break;
                case CLOCKWAIT:
                    waitForClock(void);
                    break;
                case GETSUPPORTPTR:
                    getSupportStruct(void);
                    break;
                default:
                    PANIC();
                    break;
            }
        }
    }else{
        exceptionHandler(GENERAL);
    }
}

void createProcess(){
    pcb_t* newProcess;
    newProcess->p_s = a1;
    newProcess->p_supportStruct = a2;
    insertChild(currentProcess, newProcess);
    insertProcQ(getReadyQueue(), newProcess);
    newProcess->p_time = 0;
    newProcess->p_semAdd = NULL;

}

void terminateProcess(){
    terminateRecursive(currentProcess);
    // TODO check for exception handler
    scheduler();
    return;
}

void terminateRecursive(pcb_t* p){
    if(p == NULL) return;
    if(!emptyChild(p))
        terminateRecursive(p -> p_child);
    if(p -> p_next_sib != NULL)
        terminateRecursive(p -> p_next_sib);
    outChild(p);
    if(p->p_semAdd != NULL){
        //TODO: fare il ++ solo se è negativo e non intlNo
        (*(p->p_semAdd))++;
        outBlocked(p);
        softblockCount--;
    }else{
        outProcQ(getReadyQueue(), p);
    }
    processCount--;
    freePcb(p);
    return;
}

void passeren(int* semaddr){
    (*semaddr)--;
    if(*semaddr <= 0){
        insertBlocked(semaddr, currentProcess);
        //manca gestione del time
        currentProcess=NULL;
        scheduler();
    }
    return;
}

void verhogen(int* semaddr){
    (*semaddr)++;
    if(*semaddr <= 0){
        pcb_t* tmp = removeBlocked(semaddr);
        insertProcQ(getReadyQueue(),tmp);
    }
    return;
}

void waitIO(int intlNo, int  dnum, int waitForTermRead){
    switch (intlNo)
    {
    case 3:
        passeren(semDisk[dnum]);
        break;
    case 4:
        passeren(semFlash[dnum]);
        break;
    case 5:
        passeren(semNetwork[dnum]);
        break;
    case 6:
        passeren(semPrinter[dnum]);
        break;
    case 7:
        if(waitForTermRead)
            passeren(semTerminalRecv[dnum]);
        else
            passeren(semTerminalTrans[dnum]);
        break;
    default:
        PANIC();
        break;
    }
}

// TODO + CPU time used during the current quantum/time slice;
void getCPUTime(){
    unsigned time = currentProcess->p_time;
    currentProcess->p_s.reg_v0 = time;
    return;
}

void waitForClock(){
    passeren(&semIntTimer);
    return;
}

void getSupportData(){
    currentProccess->p_s.reg_v0 = currentProcess->p_supportStruct;
    return;
}