#include <exceptions.h>
#include <umps3/umps/libumps.h>
#include <pandos_const.h>
#include <interrupts.h>
#include <initial.h>
#include <scheduler.h>
#include <syscalls.h>


/**
 * @brief this function handles both TLB Exceptions and all others types of trap
 * What type of exception is handled depends on the input passed and
 * the function behaviour is related to the current process support
 * structure value, that can exist or be NULL.
 *
 * @param index 0 (PGFAULTEXCEPT) or 1 (GENERALEXCEPT), indicating
 * the type of the exception to be handled
 */
HIDDEN void passUpOrDie(unsigned int index){

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
    int type = (EXCEPTION_STATE->cause & GETEXECCODE) >> CAUSESHIFT;
    switch (type){
        //Interrupts.
        case IOINTERRUPTS:
            interruptHandler(EXCEPTION_STATE);
            break;
        //TLB exceptions.
        case TLBMOD ... TLBINVLDS:
            TLBExcHandler();
            break;
        //General trap exceptions.
         case 4 ... 7: case 9 ... 12:
            generalTrapHandler();
            break;
        //Syscalls.
        case SYSEXCEPTION:
            sysHandler();
            break;
        default:
            PANIC();
            break;
        }
}
