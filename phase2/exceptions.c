#include <exceptions.h>

void passUpOrDie(unsigned index) {

    support_t *support = currentProcess->p_supportStruct;

    if (support != NULL) {
        support->sup_exceptState[index] = *EXCTYPE;
        context_t* context = &(supportStructure->sup_exceptContext[index]);
        LDCXT(context->c_stackPtr, context->c_status, context->c_pc);
    }
    else {
        // TODO: Gestire come Syscall 2 (kill), non so se basta usare la syscall
        //TODO: lo scheduler è chiamato chiamando sto coso
        terminateProcess();
    }
}

unsigned  exceptionType(){
    unsigned int exType = (getCause() & GETEXECCODE) >> CAUSESHIFT;
    if(exceptionNumber == 0) return IOINTERRUPTS;
    else if(exceptionNumber == 8) return SYSEXCEPTION;
    else if(exceptionNumber>=1 && exceptionNumber<=3) return TLBTRAP;
    else return GENERAL;
}

void exceptionHandler(){
    unsigned type = exceptionHandler();
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
        //TODO: ci va scheduler()?
}