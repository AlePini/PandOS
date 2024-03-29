#include <sysSupport.h>
#include <umps3/umps/arch.h>
#include <umps3/umps/libumps.h>
#include <pandos_const.h>
#include <pandos_types.h>
#include <memory.h>
#include <syscalls.h>
#include <initProc.h>
#include <vmSupport.h>

/**
 * @brief Array of semaphores, 8 for each device.
 */
SEMAPHORE deviceSemaphores[SEMNUM];


void generalExceptionHandler(){

    //Get the support struct
    support_t *support = (support_t*) SYSCALL(GETSUPPORTPTR, 0, 0, 0);
    //Get the exception type
    volatile unsigned int exType = ((support->sup_exceptState[GENERALEXCEPT].cause) & GETEXECCODE) >> CAUSESHIFT;

    //If it is a syscall handle it
    if (exType == SYSEXCEPTION){
        //Get syscall number
        volatile unsigned int sysNum = support->sup_exceptState[GENERALEXCEPT].reg_a0;
        syscallExceptionHandler(sysNum, support);
    }
    //Otherwise kill it
    else programTrapExceptionHandler(support);


}

void syscallExceptionHandler(int sysNumber, support_t *support){

    volatile unsigned int arg1 = (unsigned int) support->sup_exceptState[GENERALEXCEPT].reg_a1;
    volatile unsigned int arg2 = (unsigned int) support->sup_exceptState[GENERALEXCEPT].reg_a2;

    switch(sysNumber){
        case TERMINATE:
            terminate(support);
            break;
        case GET_TOD:
            getTOD(support);
            break;
        case WRITEPRINTER:
            writePrinter((char*) arg1, arg2, support);
            break;
        case WRITETERMINAL:
            writeTerminal((char*) arg1, arg2, support);
            break;
        case READTERMINAL:
            readTerminal((char*) arg1, support);
            break;
        default:
            programTrapExceptionHandler(support);
            break;
    }

    //Increment the program counter by 4 to avoid loops.
    support->sup_exceptState[GENERALEXCEPT].pc_epc += WORDLEN;
    //Load the current state
    LDST(&support->sup_exceptState[GENERALEXCEPT]);

}

void programTrapExceptionHandler(support_t *support) {
    terminate(support);
}

void terminate(support_t *support){

    //Mark as unused the entry
    clearSwap(support->sup_asid);

    //If the process hold the mutex on a semaphore release it
    for (int i = 0; i < SEMNUM; i++) {
        if (deviceSemaphores[i] == 0)
            SYSCALL(VERHOGEN, (memaddr) &deviceSemaphores[i], 0, 0);
    }

    //Makes a V on the masterSemaphore
    SYSCALL(VERHOGEN, (memaddr) &masterSemaphore, 0, 0);
    //Kills the process
    SYSCALL(TERMPROCESS, 0, 0, 0);
}

void getTOD(support_t *support){
    STCK(support->sup_exceptState[GENERALEXCEPT].reg_v0);
}

void writePrinter(char* string, int len, support_t* support){

    int deviceNumber = GET_DEVICE_NUMBER(support);
    int semNum = getDeviceSemaphoreIndex(PRNTINT, deviceNumber, 0);
    int charsTransmitted = 0;
    devreg_t* devReg = (devreg_t*) DEV_REG_ADDR(PRNTINT, deviceNumber);

    // Check if the address and the length are valid
    bool stringSize = (len >= 0 && len <= MAXSTRLENG);
    if(ADDRESS_IN_RANGE(string, string+len) && stringSize) {

        SYSCALL(PASSEREN, (memaddr) &deviceSemaphores[semNum], 0, 0);

        int status;

        for (int i = 0; i < len; i++) {

            DISABLEINTERRUPTS;

            devReg->dtp.data0 = ((unsigned int) *(string + i));
            devReg->dtp.command = PRINTCHR;
            status = SYSCALL(IOWAIT, PRNTINT, deviceNumber, FALSE);

            ENABLEINTERRUPTS
            if((status & TERM_STATUS_MASK) == OKCHARTRANS){
                charsTransmitted++;
            }else{
                // Error: return the negative value of the status
                charsTransmitted = -(status & TERM_STATUS_MASK);
                break;
            }
        }
        SYSCALL(VERHOGEN, (memaddr) &deviceSemaphores[semNum], 0, 0);
        support->sup_exceptState[GENERALEXCEPT].reg_v0 = charsTransmitted;

    } else terminate(support);
}

void writeTerminal(char *string, int len, support_t* support){

    int deviceNumber = GET_DEVICE_NUMBER(support);
    int semNum = getDeviceSemaphoreIndex(TERMINT, deviceNumber, 0);
    int charsTransmitted = 0;
    devreg_t* devReg = (devreg_t*) DEV_REG_ADDR(TERMINT, deviceNumber);

    // Check if the address and the length are valid
    bool stringSize = len >= 0 && len <= MAXSTRLENG;
    if (ADDRESS_IN_RANGE(string, string+len) && stringSize){
        //Get the mutex on the semaphore
        SYSCALL(PASSEREN, (memaddr) &deviceSemaphores[semNum], 0, 0);

        int status;

        for (int i = 0; i < len; i++){

            DISABLEINTERRUPTS;

            //Transm_command is divided in 2 parts:
            //we have to set both  the  command we are using and the exact character we are transmitting
            devReg->term.transm_command = (((unsigned int) *(string+i)) << BYTELENGTH | TRANSMITCHAR);    //Character + Command
            //Get the status code
            status = SYSCALL(IOWAIT, TERMINT, deviceNumber, FALSE);

            ENABLEINTERRUPTS
            if ((status & TERM_STATUS_MASK) == OKCHARTRANS){
                charsTransmitted++;
            } else {
                // Error: return the negative value of the status
                charsTransmitted = -(status & TERM_STATUS_MASK);
                break;
            }
        }
        SYSCALL(VERHOGEN, (memaddr) &deviceSemaphores[semNum], 0, 0);
        //Return the value and release the mutex on the semaphore
        support->sup_exceptState[GENERALEXCEPT].reg_v0 = charsTransmitted;

    } else terminate(support);
}

void readTerminal(char *string, support_t *support){

    int deviceNumber = GET_DEVICE_NUMBER(support);
    int semNum = getDeviceSemaphoreIndex(TERMINT, deviceNumber, 0);
    int charsTransmitted = 0;
    char received = ' ';
    devreg_t* devReg = (devreg_t*) DEV_REG_ADDR(TERMINT, deviceNumber);

    SYSCALL(PASSEREN, (memaddr) &deviceSemaphores[semNum], 0, 0);

    /*se l'indirizzo e' fuori dalla memoria virtuale si uccide*/
    while ((int)string >= KUSEG && received != EOL){

        DISABLEINTERRUPTS;

        devReg->term.recv_command = RECEIVECHAR;
        int status = SYSCALL(IOWAIT, TERMINT, deviceNumber, TRUE);

        ENABLEINTERRUPTS

        // If status code is not correct break out the while.
        if((status & TERM_STATUS_MASK) == OKCHARTRANS){
            received = (status >> BYTELENGTH);
            *string = received;
            string++;
            charsTransmitted++;
        }else {
            charsTransmitted = -(status & TERM_STATUS_MASK);
            break;
        }
    }

    SYSCALL(VERHOGEN, (memaddr) &deviceSemaphores[semNum], 0, 0);

    if((int)string >= KUSEG)
        support->sup_exceptState[GENERALEXCEPT].reg_v0 = charsTransmitted;
    else terminate(support);

}
