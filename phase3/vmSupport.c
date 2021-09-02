#include <umps3/umps/arch.h>
#include <umps3/umps/libumps.h>
#include <pandos_const.h>
#include <vmSupport.h>
#include <initProc.h>
#include <sysSupport.h>
#include <initial.h>
#include <interrupts.h>
#include <syscalls.h>

#define POOLSTART (RAMSTART + (32 * PAGESIZE))
#define PAGESHIFT 12

#define PFNSHIFT 12
#define PFNMASK 0xFFFFF000

#define NOTUSED -1
#define GETVPN(X) ((UPROCSTACKSTART <= X && X <= USERSTACKTOP) ?  (MAXPAGES-1) : ((X - VPNSTART) >> VPNSHIFT))
#define GETPFN(T) ((T - POOLSTART) >> PFNSHIFT)
#define SETPFN(TO, N) TO = (TO & ~PFNMASK) | ((N << PFNSHIFT) + POOLSTART)

swap_t swapTable[POOLSIZE];
SEMAPHORE swapPoolSemaphore;

extern pcb_t* currentProcess;
extern int deviceSemaphores[SEMNUM];
extern void generalExceptionHandler();
extern int getDeviceSemaphoreIndex(int line, int device, int read);

void initSwapStructs(){
    swapPoolSemaphore = 1;
    //Swap pool semaphore & Swap Table initialization
    for(int i=0; i<POOLSIZE; i++){
        swapTable[i].sw_asid = NOTUSED;
    }

}

void clearSwap(int asid){
    for (int i = 0; i < UPROCMAX * 2; i++){
        if (swapTable[i].sw_asid == asid)
            swapTable[i].sw_asid = -1;
    }
}

int replacementAlgorithm() {

    //Static so it stores the value of the last frame used
    static int index = 0;
    int i = 0;

    //I keep searchjing a free page, if I can't i select ther next in the list
    while((swapTable[(index + i) % POOLSIZE].sw_asid != -1) && (i < POOLSIZE))
        i++;

    if(i == POOLSIZE) i = 1;

    index = (index + i) % POOLSIZE;
    return index;

}

void updateTLB(pteEntry_t *newEntry){
    TLBCLR();
}

void executeFlashAction(int deviceNumber, unsigned int pageIndex, unsigned int command, support_t *support) {

    memaddr address = (pageIndex * PAGESIZE) + POOLSTART;
    int semNum = getDeviceSemaphoreIndex(FLASHINT, deviceNumber, 0);
    // Obtain the mutex on the device
    SYSCALL(PASSEREN, (memaddr) &deviceSemaphores[semNum], 0, 0);
    devreg_t* flashDevice = (devreg_t*) DEV_REG_ADDR(FLASHINT, deviceNumber);
    flashDevice->dtp.data0 = address;

    DISABLEINTERRUPTS;

    flashDevice->dtp.command = command;

    int deviceStatus = SYSCALL(IOWAIT, FLASHINT, deviceNumber, FALSE);

    ENABLEINTERRUPTS

    // Release the mutex
    SYSCALL(VERHOGEN, (memaddr) &deviceSemaphores[semNum], 0, 0);

    if (deviceStatus != 1) {
        SYSCALL(VERHOGEN, (memaddr) &swapPoolSemaphore, 0, 0);

        //If something didn't work create a trap and kill the process
        programTrapExceptionHandler(support);
    }
}


void readFlash(int deviceNumber, unsigned int blockIndex, unsigned int pageIndex, support_t *support) {
    executeFlashAction(deviceNumber, pageIndex, (FLASHREAD | (blockIndex << 8)), support);
}


void writeFlash(int deviceNumber, unsigned int blockIndex, unsigned int pageIndex, support_t *support) {
    executeFlashAction(deviceNumber, pageIndex, (FLASHWRITE | (blockIndex << 8)), support);
}

void pager(){

    support_t *support = (support_t *) SYSCALL(GETSUPPORTPTR, 0, 0, 0);
    //Chekc the cause of the exception, if it is a TLB-Modification I kill it
    if((support->sup_exceptState[PGFAULTEXCEPT].cause & GETEXECCODE) >> CAUSESHIFT == 1){
        generalExceptionHandler(support);
    }

    SYSCALL(PASSEREN, (memaddr) &swapPoolSemaphore, 0, 0);

    //Get missing page number infos
    unsigned int pageNum = GETVPN(support->sup_exceptState[PGFAULTEXCEPT].entry_hi);;
    unsigned int id = support->sup_asid;

    int i = replacementAlgorithm();

    //If it is occupied get its informations
    if(swapTable[i].sw_asid != -1){
        int occupiedPageNum = swapTable[i].sw_pageNo;
        int occupiedId = swapTable[i].sw_asid;

        DISABLEINTERRUPTS;

        //Mark the entry as not valid
        pteEntry_t *occupiedEntry = swapTable[i].sw_pte;
        occupiedEntry->pte_entryLO &= ~VALIDON;
        updateTLB(occupiedEntry);

        //Restores the previous status.
        ENABLEINTERRUPTS

        // Update process x's backing store
        if(occupiedEntry->pte_entryLO & DIRTYON){
            writeFlash(occupiedId-1, occupiedPageNum, i, support);
        }
    }

        // Read the contents from the flash device
        readFlash(id-1, pageNum, i, support);

        DISABLEINTERRUPTS;

        swapTable[i].sw_asid = id;
        swapTable[i].sw_pageNo = pageNum;
        swapTable[i].sw_pte = &(support->sup_privatePgTbl[pageNum]);

        // Update the process' page table
        unsigned int pageAddress = POOLSTART + (i * PAGESIZE);
        support->sup_privatePgTbl[pageNum].pte_entryLO = pageAddress | VALIDON | DIRTYON;

        // Update the TLB
        updateTLB(&(support->sup_privatePgTbl[i]));

        //Restores the previous status.
        ENABLEINTERRUPTS

        SYSCALL(VERHOGEN, (memaddr) &swapPoolSemaphore, 0, 0);

        LDST((state_t *) &(support->sup_exceptState[PGFAULTEXCEPT]));
}


void uTLB_RefillHandler() {

    volatile unsigned int pageNum;
    pageNum = GETVPN(EXCEPTION_STATE->entry_hi);

    pteEntry_t *newEntry = &(currentProcess->p_supportStruct->sup_privatePgTbl[pageNum]);

    setENTRYHI(newEntry->pte_entryHI);
    setENTRYLO(newEntry->pte_entryLO);
    TLBWR();

    returnControl();
}