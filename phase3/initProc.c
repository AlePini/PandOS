#include <umps3/umps/libumps.h>
#include <pandos_const.h>
#include <pandos_types.h>
#include <initProc.h>
#include <sysSupport.h>
#include <vmSupport.h>

SEMAPHORE masterSemaphore;
extern int deviceSemaphores[SEMNUM];
extern void pager();
extern void generalExceptionHandler();

HIDDEN support_t supportStructs[UPROCMAX+1];

void initializeSemaphores(){

    masterSemaphore = 0;

    //Semaphores Initialization
    for (int i = 0; i < SEMNUM; i++){
        deviceSemaphores[i] = 1;
    }
}

void initializeProcesses(){

    //Preparing a common status to all processes
    state_t pState;
    pState.pc_epc = UPROCSTARTADDR;
    pState.reg_t9 = UPROCSTARTADDR;
    pState.reg_sp = USERSTACKTOP;
    pState.status = IEPON | IMON | TEBITON | USERPON;

    for(int id=1; id<= UPROCMAX; id++){
        pState.entry_hi = (id << ASIDSHIFT);
        //Stack pointer calculation
        memaddr ramtop;
        RAMTOP(ramtop);
        memaddr topStack = ramtop - (2*id*PAGESIZE);

        //Support struct setup
        supportStructs[id].sup_asid = id;
        supportStructs[id].sup_exceptContext[PGFAULTEXCEPT].c_stackPtr = (memaddr) (topStack + PAGESIZE);
        supportStructs[id].sup_exceptContext[GENERALEXCEPT].c_stackPtr = (memaddr) topStack;
        supportStructs[id].sup_exceptContext[PGFAULTEXCEPT].c_status = IMON | IEPON | TEBITON;
        supportStructs[id].sup_exceptContext[GENERALEXCEPT].c_status = IMON | IEPON | TEBITON;
        supportStructs[id].sup_exceptContext[PGFAULTEXCEPT].c_pc = (memaddr) &pager;
        supportStructs[id].sup_exceptContext[GENERALEXCEPT].c_pc = (memaddr) &generalExceptionHandler;

        //Page tables setup
        for (int i = 0; i < USERPGTBLSIZE-1; i++){
            //VPN and ASID setup
            supportStructs[id].sup_privatePgTbl[i].pte_entryHI = VPNSTART + (i << VPNSHIFT) + (id << ASIDSHIFT); //VPN and ASID setup
            supportStructs[id].sup_privatePgTbl[i].pte_entryLO = DIRTYON; //Dirty bit
        }

        //Stack setup
        supportStructs[id].sup_privatePgTbl[USERPGTBLSIZE-1].pte_entryHI = UPROCSTACKSTART + (id << ASIDSHIFT); //VPN and ASID setup
        supportStructs[id].sup_privatePgTbl[USERPGTBLSIZE-1].pte_entryLO = DIRTYON;   //Dirty Bit

        //Create the process with the state and support prepared
        SYSCALL(CREATEPROCESS, (memaddr) &pState, (memaddr) &(supportStructs[id]), 0);
    }
}

void handleMasterSemaphore(){
    for (int i = 0; i < UPROCMAX; i++){
        SYSCALL(PASSEREN, (memaddr) &masterSemaphore, 0, 0);
    }
}

void test(){
    //Initialize swap structs and semaphores
    initSwapStructs();
    initializeSemaphores();
    //Initialize processes
    initializeProcesses();
    //Doing P on master semaphore
    handleMasterSemaphore();
    SYSCALL(TERMPROCESS, 0, 0, 0);

}
