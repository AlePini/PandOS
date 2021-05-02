#include <sysSupport.h>

HIDDEN int exceptionType(){
    SYSCALL(GETSUPPORTPTR, 0, 0, 0);
    state_t exceptionState = EXCEPTION_STATE->reg_v0->sup_exceptState[GENERALEXCEPT];
    unsigned int exceptionType = (exceptionState->cause & GETEXECCODE) >> CAUSESHIFT;
    if(exceptionType >= 9 && exceptionType <=13) return SYS_EX;
    else return PROGRAM_TRAP_EX;
}

void generalExceptionHandler(){

    //Increment the program counter by 4 to avoid loops.
    EXCEPTION_STATE->pc_epc += WORDLEN;
    int type = exceptionType();
    switch (type){
        //Syscalls.
        case SYS_EX:
            syscallExceptionHandler();
            break;
        //Program Traps.
        case PROGRAM_TRAP_EX:
            programTrapHandler();
            break;
}

void syscallExceptionHandler(){

    state_t exceptionState = EXCEPTION_STATE->reg_v0->sup_exceptState[GENERALEXCEPT];

    //Reads the id of the syscall.
    unsigned sysNum = exceptionState->reg_a0;


    //Retrieve the informations of the syscall from the registers.
    unsigned arg1 = exceptionState.reg_a1;
    unsigned arg2 = exceptionState.reg_a2;
    unsigned arg3 = exceptionState.reg_a3;

    switch(sysNum){
                case TERMINATE:
                    terminate();
                    break;
                case GET_TOD:
                    getTod();
                    break;
                case WRITEPRINTER:
                    writePrinter((char*) arg1, (int) len);
                    break;
                case WRITETERMINAL:
                    writeTerminal((char*) arg1, (int) len);
                    break;
                case READTERMINAL:
                    readTerminal((char*) arg1);
                    break;
                default:
                    programTrapExceptionHandler();
                    break;
            }
            //TODO: ci va un LDST o no?
}

void programTrapExceptionHandler(){
    terminate();
}