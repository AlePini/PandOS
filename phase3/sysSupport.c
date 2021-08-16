#include <sysSupport.h>

#include <memory.h>
#include <syscall.h>
#include <pandos_const.h>
#include <umps3/umps/arch.h>

#define printerSem 3
#define writeTerminalSem 5
#define readTerminalSem 4
#define GETTERMSTATUS 0xFF
#define GET_DEVICE_NUMBER(S) S->sup_asid - 1
#define TERMSHIFT 8
#define PRINTCHR 2
#define RECEIVECHAR 2
#define DEVICE_READY 5
#define KUSEG_START 0x80000000
#define TEXT_AND_DATA_TOP 0x8001E000
#define UPROCSTACKSTART 0xBFFFF000
#define EOL '\n'

#define ADDRESS_IN_RANGE(S, E) (S >= ( (char *) KUSEG) && (E <= (char *) TEXT_AND_DATA_TOP) || S >= (char *) UPROCSTACKSTART) && (E <= (char *) USERSTACKTOP)

int deviceSemaphores[SUPP_SEM_NUMBER][UPROCMAX];
extern int masterSemaphore;
extern void clearSwap();
extern pcb_t currentProcess;

void generalExceptionHandler(){

    //Get the support struct
    support_t *support = (support_t*) SYSCALL(GETSUPPORTPTR, 0, 0, 0);
    //Get the exception type
    unsigned int exType = ((support->sup_exceptState[GENERALEXCEPT].cause) & GETEXECCODE) >> CAUSESHIFT;

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

    //Increment the program counter by 4 to avoid loops.
    support->sup_exceptState[GENERALEXCEPT].pc_epc += WORDLEN;

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
                    writePrinter((char*) arg1, (int) arg2, support);
                    break;
                case WRITETERMINAL:
                    writeTerminal((char*) arg1, (int) arg2, support);
                    break;
                case READTERMINAL:
                    readTerminal((char*) arg1, support);
                    break;
                default:
                    programTrapExceptionHandler(support);
                    break;
            }

    //Load the current state
    LDST(&support->sup_exceptState[GENERALEXCEPT]);

}

void programTrapExceptionHandler(support_t *support) {
    terminate(support);
}

void terminate(support_t *support){

    //Get the device number
    int deviceNumber = GET_DEVICE_NUMBER(support);

    //If the process hold the mutex on a semaphore release it
    for (int i = 0; i < SUPP_SEM_NUMBER; i++) {
        if (deviceSemaphores[i][deviceNumber] <= 0)   //In teoria non scende mai sotto lo 0
            SYSCALL(VERHOGEN, (memaddr) &deviceSemaphores[i][deviceNumber], 0, 0);
    }

    //Mark as unused the entry
    clearSwap(support->sup_asid);

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
    int charsTransmitted = 0;
    devreg_t* devReg = (devreg_t*) DEV_REG_ADDR(PRNTINT, deviceNumber);

    // Check if the address and the length are valid
    bool stringSize = (len >= 0 && len <= MAXSTRLENG);
    if(ADDRESS_IN_RANGE(string, string+len) && stringSize) {
        SYSCALL(PASSEREN, (memaddr) &deviceSemaphores[printerSem][deviceNumber], 0, 0);

        for (int i = 0; i < len; i++) {
            if(devReg->dtp.status == READY) {
                //Disabling interrupts
                setSTATUS(getSTATUS() & (~IECON));

                devReg->dtp.data0 = ((unsigned int) *(string + i));
                devReg->dtp.command = PRINTCHR;
                SYSCALL(IOWAIT, PRNTINT, deviceNumber, FALSE);
                //Re-enabling interrupts
                setSTATUS(getSTATUS() | IECON);
                charsTransmitted++;
            }else{
                // Error: return the negative value of the status
                charsTransmitted = -(devReg->dtp.status);
                break;
            }
        }
        support->sup_exceptState[GENERALEXCEPT].reg_v0 = charsTransmitted;
        SYSCALL(VERHOGEN, (memaddr) &deviceSemaphores[printerSem][deviceNumber], 0, 0);
    }
    else terminate(support);
}

void writeTerminal(char *string, int len, support_t* support){

    unsigned int statusCode = OKCHARTRANS;
    int deviceNumber = GET_DEVICE_NUMBER(support);
    int charsTransmitted = 0;
    volatile devreg_t* devReg = (devreg_t*) DEV_REG_ADDR(TERMINT, deviceNumber);

    // Check if the address and the length are valid
    bool stringSize = len >= 0 && len <= MAXSTRLENG;
    if(ADDRESS_IN_RANGE(string, string+len) &&  stringSize) {
        //Get the mutex on the semaphore
        SYSCALL(PASSEREN, (memaddr) &deviceSemaphores[readTerminalSem][deviceNumber], 0, 0);
        for (int i = 0; i < len; i++) {
            if(((devReg->term.transm_status & GETTERMSTATUS) == DEVICE_READY) && (statusCode & GETTERMSTATUS) == OKCHARTRANS){

                //Disabling interrupts
                setSTATUS(getSTATUS() & (~IECON));

                //Transm_command is divided in 2 parts:
                //we have to set both  the  command we are using and the exact character we are transmitting
                devReg->term.transm_command = (unsigned int) *(string+i) << BYTELENGTH;    //Character
                devReg->term.transm_command |= TRANSMITCHAR;    //Command

                //Get the status code
                statusCode = SYSCALL(IOWAIT, TERMINT, deviceNumber, FALSE);

                //Re-enabling interrupts
                setSTATUS(getSTATUS() | IECON);
                charsTransmitted++;
            }
            else {
                // Error: return the negative value of the status
                charsTransmitted = -statusCode;
                break;
            }
        }
        //Return the value and release the mutex on the semaphore
        support->sup_exceptState[GENERALEXCEPT].reg_v0 = charsTransmitted;
        SYSCALL(VERHOGEN, (memaddr) &deviceSemaphores[writeTerminalSem][deviceNumber], 0, 0);
    }
    else terminate(support);
}

void readTerminal(char *buffer, support_t *support){

    int deviceNumber = GET_DEVICE_NUMBER(support);
    int charsTransmitted = 0;
    int statusCode = 0;
    devreg_t* devReg = DEV_REG_ADDR(TERMINT, deviceNumber);
    char received = ' ';

    SYSCALL(PASSEREN, (memaddr) &deviceSemaphores[readTerminalSem][deviceNumber], 0, 0);

    while(ADDRESS_IN_RANGE(buffer, buffer) && ((devReg->term.recv_status & TERM_STATUS_MASK) == DEVICE_READY) ){

        //Disable interrupts
        setSTATUS(getSTATUS() & (~IECON));

        devReg->term.recv_command = RECEIVECHAR;
        statusCode = SYSCALL(IOWAIT, TERMINT, deviceNumber, TRUE);

        //Renable interruputs
        setSTATUS(getSTATUS() | IECON);
        // If status code is not correct break out the while.
        if((statusCode & TERM_STATUS_MASK) != OKCHARTRANS) break;
        //Receiving the characters
        received = statusCode >> BYTELENGTH;
        if(received != EOL) {
            *buffer = received;
            buffer++;
            charsTransmitted++;
        } else break;
    }

    //If there's an error:
    if((devReg->term.recv_status & TERM_STATUS_MASK) != DEVICE_READY || (statusCode & TERM_STATUS_MASK) != OKCHARTRANS) {
        charsTransmitted = -(statusCode);
    }

    SYSCALL(VERHOGEN, (memaddr) &deviceSemaphores[readTerminalSem][deviceNumber], 0, 0);

    //If the address is not valid terminate the process otherwise return the correct value
    if(ADDRESS_IN_RANGE(buffer, buffer)){
        support->sup_exceptState[GENERALEXCEPT].reg_v0 = charsTransmitted;
    }else terminate(support);
}
