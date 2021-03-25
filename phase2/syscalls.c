#include <syscalls.h>
#include <scheduler.h>
#include <utils.h>
#include <exceptions.h>


void sysHandler(){
    //Leggo l'id della syscall
    unsigned sysdnum = EXCTYPE -> reg_a0;
    if(sysdnum >= 1 && sysdnum <= 8){
        currentProcess->p_s = *EXCTYPE;
        //Se è fra 1 e 8 devo essere in kernel mode altrimenti eccezione
        if(CHECK_USERMODE(currentProcess->p_s.status)){
            setCAUSE(EXC_RI<<CAUSESHIFT);
            exceptionHandler();
        }
        else{
            //Se va tutto bene controllo che syscall sia
            unsigned arg1 = currentProcess->p_s.reg_a1;
            unsigned arg2 = currentProcess->p_s.reg_a2;
            unsigned arg3 = currentProcess->p_s.reg_a3;

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
    pcb_t* newProcess;
    newProcess->p_s = *arg1;
    newProcess->p_supportStruct = arg2;
    insertChild(currentProcess, newProcess);
    insertProcQ(&readyQueue, newProcess);
    newProcess->p_time = 0;
    newProcess->p_semAdd = NULL;
}

void terminateProcess(){
    terminateRecursive(currentProcess);
    scheduler();
    return;
}

void terminateRecursive(pcb_t* p){
    if(p == NULL) return;
    if(!emptyChild(p))
        terminateRecursive(p->p_child);
    if(p -> p_next_sib != NULL)
        terminateRecursive(p->p_next_sib);
    outChild(p);
    if(p->p_semAdd != NULL){
        softblockCount--;
        outBlocked(p);
        //TODO: fare il ++ solo se è negativo e non intlNo
        (*(p->p_semAdd))++;
    }else{
        outProcQ(&readyQueue, p);
    }
    processCount--;
    freePcb(p);
    return;
}

void passeren(int* semaddr){
    (*semaddr)--;
    if(*semaddr <= 0){
        STCK(endTimeSlice);
        currentProcess->p_time+=(endTimeSlice-startTimeSlice);
        //TODO:Ti salvi lo stato attuale delle cose per poi bloccarlo, così che quando ripiglia ricomincia da qui
        currentProcess->p_s = *EXCTYPE;
        insertBlocked(semaddr, currentProcess);
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
        //Prendo la linea di interrupt(rimappandole da 0 a 4) e la moltiplico per 8. Se è un terminal line controllo se leggo e trasmetto
    int startingPoint = 8*(intlNo+waitForTermRead-3);
    passeren(semaphoreList[startingPoint + dnum]);
    // switch (intlNo)
    // {
    // case 3:
    //     //Sarebbero da 0-7
    //     passeren(semaphoreList[dnum])
    //     //passeren(semDisk[dnum]);
    //     break;
    // case 4:
    //     //Sarebbero da 8-15
    //     passeren(semFlash[8+dnum]);
    //     break;
    // case 5:
    //     //Sarebbero da 16-23
    //     passeren(semNetwork[16+dnum]);
    //     break;
    // case 6:
    //     //Sarebbero da 24-31
    //     passeren(semPrinter[24+dnum]);
    //     break;
    // case 7:
    //     if(waitForTermRead)
    //         //Sarebbero da 32-39
    //         passeren(semTerminalRecv[32+dnum]);
    //     else
    //         //Sarebbero da 40-47
    //         passeren(semTerminalTrans[40+dnum]);
    //     break;
    // default:
    //     PANIC();
    //     break;
    // }
}

void getCpuTime(){
    STCK(endTimeSlice);
    //currentProcess->p_time+=(endTimeSlide-startTimeSlice);
    unsigned time = currentProcess->p_time + (endTimeSlice-startTimeSlice);
    //TODO: non penso vada nel current process ma bensì nel BIOSDATAPAGE poichè tu fai LDST su quello li
    //currentProcess->p_s.reg_v0 = time;
    //TODO: non so se scritto così vada bene
    EXCTYPE->reg_v0 = time;
    return;
}

void waitForClock(){
    passeren(&semIntTimer);
    return;
}

void getSupportStruct(){
    //TODO: va bene?
     EXCTYPE->reg_v0 = currentProcess->p_supportStruct;
    return;
}