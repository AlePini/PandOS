#include <exceptions.h>
#include <scheduler.h>
#include <syscalls.h>

void passUpOrDie(unsigned index) {

    support_t *support = currentProcess->p_supportStruct;

    if (support != NULL) {
        //TODO: vedere se va bene o meno, dipende se extype quindi biosdatapage si aggiorna davvero da solo
        support->sup_exceptState[index] = *EXCTYPE;
        context_t* context = &(support->sup_exceptContext[index]);
        LDCXT(context->c_stackPtr, context->c_status, context->c_pc);
    }
    else {
        // TODO: Gestire come Syscall 2 (kill), non so se basta usare la syscall
        terminateProcess();
    }
}

unsigned  exceptionType(){
    state_t *exceptionState = EXCTYPE;
    unsigned int exType = (exceptionState->cause & GETEXECCODE) >> CAUSESHIFT;
    if(exType == 0) return IOINTERRUPTS;
    else if(exType == 8) return SYSEXCEPTION;
    else if(exType>=1 && exType<=3) return TLBTRAP;
    else return GENERAL;
}

void exceptionHandler(){
    unsigned type = exceptionType();
    switch (type){
        case IOINTERRUPTS:
            interruptHandler();
            break;
        case SYSEXCEPTION:
            sysHandler();
            break;
        case TLBTRAP:
            passUpOrDie(PGFAULTEXCEPT);
            break;
         case GENERAL:
            passUpOrDie(GENERALEXCEPT);
            break;
        default:
            PANIC();
            break;
        }
}