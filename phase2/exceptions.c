#include <umps3/umps/libumps.h>
#include <pandos_const.h>
#include <exceptions.h>
#include <initial.h>
#include <interrupts.h>
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
HIDDEN void passUpOrDie(unsigned index) {

    support_t *support = currentProcess->p_supportStruct;

    //If the support struct exists execute the "Pass Up".
    if (support != NULL) {
        support->sup_exceptState[index] = *EXCEPTION_STATE;
        context_t* context = &(support->sup_exceptContext[index]);
        LDCXT(context->c_stackPtr, context->c_status, context->c_pc);
    }
    //otherwise the "Die".
    else {
        terminateProcess();
    }
}

/**
 * @brief this function returns what type of exception
 * is raised by checking the BIOSDATAPAGE.
 * 
 * @return the type of exception among the possible four.
 */
HIDDEN int exceptionType(){
    state_t *exceptionState = EXCEPTION_STATE;
    unsigned int exType = (exceptionState->cause & GETEXECCODE) >> CAUSESHIFT;
    if(exType == 0) return IOINTERRUPTS;
    else if(exType == 8) return SYSEXCEPTION;
    else if(exType>=1 && exType<=3) return TLBTRAP;
    else if((exType>=4 && exType <=7) || (exType >= 9 && exType <= 12))
        return GENERAL;
    else return ERROR_TYPE;
}

void TLBExcHandler() {
	passUpOrDie(PGFAULTEXCEPT);
}

void generalTrapHandler() {
	passUpOrDie(GENERALEXCEPT);
}

void exceptionHandler(){
    //Increment the program counter by 4 to avoid loops.
    EXCEPTION_STATE->pc_epc += WORDLEN;
    int type = exceptionType();
    switch (type){
        //Interrupts.
        case IOINTERRUPTS:
            interruptHandler(EXCEPTION_STATE);
            break;
        //Syscalls.
        case SYSEXCEPTION:
            sysHandler();
            break;
        //TLB exceptions.
        case TLBTRAP:
            TLBExcHandler();
            break;
        //General trap exceptions.
         case GENERAL:
            generalTrapHandler();
            break;
        default:
            PANIC();
            break;
        }
}