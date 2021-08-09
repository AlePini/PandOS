#include <sysSupport.h>
#include <syscall.h>

#define printer 3
#define writeTerminal 5
#define readTerminal 4
#define GETTERMSTATUS 0xFF
#define GET_DEVICE_NUMBER(S) S->sup_asid - 1
#define TERMSHIFT 8
#define PRINTCHR 2
#define RECEIVECHAR 2
#define DEVICE_READY 1
#define KUSEG_START 0x8000.0000
#define TEXT_AND_DATA_TOP 0x8001.E000
#define UPROCSTACKSTART 0xBFFFF000
#define EOL '\n'

int deviceSemaphores[SUPP_SEM_NUMBER][UPROCMAX];

#define ADDRESS_IN_RANGE(S, E) = (S >= ( (char *) KUSEG) && (E <= (char *) TEXT_AND_DATA_TOP) || S >= (char *) UPROCSTACKSTART) && (E <= (char *) USERSTACKTOP);


void generalExceptionHandler(){

    //Get the support struct
    support_t *support = SYSCALL(GETSUPPORTPTR, 0, 0, 0);

    //Get the exception state
    //Don't need to do all the procedure did in the nucleus level because there are 2 options:
    //Either this is a syscall 9<= x <= 13 or is a trap.
    volatile unsigned int sysNumber = support->sup_exceptState[GENERALEXCEPT].reg_a0;

    if(9 <= sysNumber <= 13) syscallExceptionHandler(sysNumber, support);
    else trapExceptionHandler(support);
}

void syscallExceptionHandler(int sysNumber, support_t *support){

    //Reads the id of the syscall.
    state_t supportState = support->sup_exceptState[GENERALEXCEPT]

    volatile unsigned int arg1 = (unsigned int) supportState.reg_a1;
    volatile unsigned int arg2 = (unsigned int) supportState.reg_a2;

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

    //Increment the program counter by 4 to avoid loops.
    support->sup_exceptState[GENERALEXCEPT].pc_epc += WORDLEN;
    //Load the current state
    LDST(&support->sup_exceptState[GENERALEXCEPT])

}

void programTrapExceptionHandler(support_t *support) {
    terminate(support);
}

void terminate(support_t *support){

    int deviceNumber = GET_DEVICE_NUMBER(support)

    for (int i = 0; i < SUPP_SEM_NUMBER; i++) {
        if (deviceSemaphores[i][deviceNumber] <= 0)    //In teoria non scende mai sotto lo 0
            SYSCALL(VERHOGEN, (memaddr) &deviceSemaphores[i][deviceNumber], 0, 0);
    }

    // // Mark as unused all the pages
    // for (int i = 0; i < USERPGTBLSIZE; i++){
    //     if(currentSupport->sup_privatePgTbl[i].pte_entryLO & VALIDON){
    //         setSTATUS(getSTATUS() & (~IECON));
    //         currentSupport->sup_privatePgTbl[i].pte_entryLO &= ~VALIDON;
    //         updateTLB(&currentSupport->sup_privatePgTbl[i]);
    //         setSTATUS(getSTATUS() | IECON)
    //     }
    // }

    //TODO: masterSemaphore e deallocare il support ottimizzazione
    SYSCALL(TERMPROCESS, 0, 0, 0);
}

void getTOD(support_t *support){
    STCK(support->sup_exceptState[GENERALEXCEPT]->reg_v0);
}


void writePrinter((char*) arg1, (int) len,support_t* support){

    int deviceNumber = GET_DEVICE_NUMBER(support);
    devreg_t* devReg = DEV_REG_ADDR(7, deviceNumber);
    int charsTransmitted = 0;

    // Check if the address and the length are valid
    bool stringSize = (len >= 0 && len <= MAXSTRLENG);
    if(ADDRESS_IN_RANGE(virtAddr, virtAddr+len) &&  stringSize) {
        SYSCALL(PASSEREN, (memaddr) &deviceSemaphores[writeTerminal][deviceNumber], 0, 0);
        for (int i = 0; i < len; i++) {
            if(devReg->dtp.status == DEVICE_READY) {
                //Disabling interrupts
                setSTATUS(getSTATUS() & (~IECON))
                devReg->dtp.data0 = ((unsigned int) *(virtAddr + i));
                devReg->dtp.command = PRINTCHR;
                SYSCALL(IOWAIT, 6, deviceNumber, 0);

                //Re-enabling interrupts
                setSTATUS(getSTATUS() | IECON)
                charsTransmitted++;
            }
            else {
                // Error: return the negative value of the status
                charsTransmitted = -(devReg->dtp.status);
                break;
            }
        }

        currentSupport->sup_exceptState[GENERALEXCEPT].reg_v0 = charsTransmitted;
        SYSCALL(VERHOGEN, (memaddr) &deviceSemaphores[printer][devNumber], 0, 0);
    }
    else {
        terminate(currentSupport);
    }
}

void writeToTerminal(char *virtAddr, int len, support_t* support){

    int deviceNumber = GET_DEVICE_NUMBER(support);
    volatile devreg_t* devReg = DEV_REG_ADDR(8, deviceNumber);

    int charsTransmitted = 0;
    unsigned int statusCode = OKCHARTRANS;

    // Check if the address and the length are valid
    bool stringSize = len >= 0 && len <= MAXSTRLENG;
    if(ADDRESS_IN_RANGE(virtAddr, virtAddr+len) &&  stringSize) {
        //Get the mutex on the semaphore
        SYSCALL(PASSEREN, (memaddr) &deviceSemaphores[writeTerminal][deviceNumber], 0, 0);
        for (int i = 0; i < len; i++) {
            if(((devReg->term.transm_status & GETTERMSTATUS) == DEVICE_READY) && (statusCode & GETTERMSTATUS) == OKCHARTRANS){
                //Disabling interrupts
                setSTATUS(getSTATUS() & (~IECON))

                //Transm_command is divided in 2 parts:
                //we have to set both  the  command we are using and the exact character we are transmitting
                devReg->term.transm_command = (unsigned int) *(virtAddr+i) << TERMSHIFT;    //Character
                devReg->term.transm_command |= TRANSMITCHAR;    //Command

                //Get the status code
                statusCode = SYSCALL(IOWAIT, 7, deviceNumber, FALSE);
                //Re-enabling interrupts
                setSTATUS(getSTATUS() | IECON)
                //We always add one to charsTransmitted because if the status is not "OKCHAR"
                //the next iteration it gets blocked and we put the negative of the status code anyway
                charsTransmitted++;
            }
            else {
                // Error: return the negative value of the status
                charsTransmitted = -statusCode;
                break;
            }
        }
        //Return the value and release the mutex on the semaphore
        currentSupport->sup_exceptState[GENERALEXCEPT].reg_v0 = charsTransmitted;
        SYSCALL(VERHOGEN, (memaddr) &deviceSemaphores[writeTerminal][devNumber], 0, 0);
    }
    else terminate(currentSupport);
}

void readTerminal(char *buffer, support_t *currentSupport){

    int deviceNumber = GET_DEVICE_NUMBER(support);
    devreg_t* devReg = DEV_REG_ADDR(8, deviceNumber);

    int charsTransmitted = 0;
    int statusCode;
    char received = '';

    SYSCALL(PASSEREN, (memaddr) &deviceSemaphores[readTerminal][devNumber], 0, 0);

    while(ADDRESS_IN_RANGE(buffer, buffer) && ((devReg->term.recv_status & TERMSTATUSMASK) == DEVICE_READY) ) {
        setSTATUS(getSTATUS() & (~IECON));
        devReg->term.recv_command = RECEIVECHAR;
        statusCode = SYSCALL(IOWAIT, 7, deviceNumber, TRUE);
        setSTATUS(getSTATUS() | IECON);
        // If status code is not correct break out the while.
        if((statusCode & TERMSTATUSMASK) != OKCHARTRANS) break;
        received = (statusCode >> TERMSHIFT);
        if(received != EOL) {
            *buffer = recvd;
            buffer++;
            charsTransmitted++;
        } else break;
    }

    //If exited the code through a break
    if((devReg->term.recv_status & TERMSTATUSMASK) != DEVICE_READY || (statusCode & TERMSTATUSMASK) != OKCHARTRANS) {
        retValue = -(statusCode);
    }

    SYSCALL(VERHOGEN, (memaddr) &deviceSemaphores[readTerminal][devNumber], 0, 0);

    //If the buffer is not in the allowed range terminate the process otherwise return
    if(CHECKADDR(buffer, buffer)){
        currentSupport->sup_exceptState[GENERALEXCEPT].reg_v0 = retValue;
    }
    else{
        terminate(currentSupport);
    }
}