#include <exceptions.h>
#include <pandos_const.h>
#include <pandos_types.h>
#include <syscalls.h>
#include <interrupts.h>
#include <initial.h>

extern pcb_PTR currentProcess;    //current process


void passUpOrDie(unsigned int index){
    support_t *support = currentProcess->p_supportStruct;

    //If the support struct exists execute the "Pass Up".
    if (support != NULL) {
        support->sup_exceptState[index] = *EXCEPTION_STATE;
        context_t* context = &(support->sup_exceptContext[index]);
        LDCXT(context->c_stackPtr, context->c_status, context->c_pc);
    }
    //otherwise the "Die".
    else
        terminateProcess();
}

void TLBExcHandler(){
	passUpOrDie(PGFAULTEXCEPT);
}

void generalTrapHandler(){
	passUpOrDie(GENERALEXCEPT);
}

void exceptionHandler(){
    /*Preleva il campo ExcCode*/
    int type = (EXCEPTION_STATE->cause & 0x3C) >> CAUSESHIFT;

    switch(type){
        case 0:                     //interrupt
            interruptHandler(EXCEPTION_STATE);
            break;
        case 1 ... 3:               //TLB
            passUpOrDie(PGFAULTEXCEPT);
            break;
        case 4 ... 7: case 9 ... 12: //trap
            passUpOrDie(GENERALEXCEPT);
            break;
        case 8:                     //syscall
            /*Il controllo passa all'syscall handler*/
            sysHandler();
            break;
    }
}
