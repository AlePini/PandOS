#include <syscall.h>
#include <scheduler.h>
#include <utils.h>
#include <exception.h>


void Terminate_process(pcb_t* p){
    if(p == NULL) return;
    if(!emptyChild(p)) Terminate_process(p -> p_child);
    if(p -> p_next_sib != NULL) Terminate_process(p -> p_next_sib);
    outChild(p);
    outProcQ(getReadyQueue(), p);
    /* Da fare verifica sul semaforo se è negativo e non device bisogna aggiustarne il valore */
    outBlocked(p);
    freePcb(p); 


void sysHandler(){
    //Leggo l'id della syscall
    unsigned sysNumber = current->p_s.reg_a0;
    if(sysNumber >=1 && sysNumber<=8){
        //Se è fra 1 e 8 devo essere in kernel mode altrimenti eccezione
        currentProcess->p_s = EXCTYPE;
        if(CHECK_USERMODE(currentProcess->p_s.status)){
            setCause(EXC_RI<<CAUSESHIFT);
            exceptionHandler(exceptionType());
        }
        else{
            //Se va tutto bene controllo che syscall sia
            unsigned arg1 = current->p_s.reg_a1;
            unsigned arg2 = current->p_s.reg_a2;
            unsigned arg3 = current->p_s.reg_a3;
            currentProcess->p_s.pc_epc = current->p_s.pc_epc + 4;
            switch(sysNumber){
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

void passeren(int* semaddr){
    (*semaddr)--;
    if(*semaddr <= 0){
        insertBlocked(semaddr, currentProcess);
        //manca gestione del time
        currentProcess=NULL;
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
