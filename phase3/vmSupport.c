#include <umps3/umps/arch.h>
#include <umps3/umps/libumps.h>
#include <pandos_const.h>
#include <vmSupport.h>
#include <initProc.h>
#include <sysSupport.h>
#include <initial.h>
#include <interrupts.h>
#include <syscalls.h>

#define HEADERPAGE 0
#define HEADERTEXTSIZE 0x0014
#define HEADERDATASIZE 0x0014

#define POOLSTART (RAMSTART + (32 * PAGESIZE))
#define PAGESHIFT 12
#define FLASH 1
#define PRINT 3
#define TREAD 4
#define TWRITE 5

#define PFNSHIFT 12
#define PFNMASK 0xFFFFF000

#define NOTUSED -1
#define GETVPN(X) ((UPROCSTACKSTART <= X && X <= USERSTACKTOP) ?  (MAXPAGES-1) : ((X - VPNSTART) >> VPNSHIFT))
#define GETPFN(T) ((T - POOLSTART) >> PFNSHIFT)
#define SETPFN(TO, N) TO = (TO & ~PFNMASK) | ((N << PFNSHIFT) + POOLSTART)

//Forse della roba va volatile
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

    //Finchè li trovo pieni e non ho gia fatto un giro completo di tutti i frame
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

    // Disabling interrupt doesn't interfere with SYS5, since SYSCALLS aren't interrupts
    setSTATUS(getSTATUS() & (~IECON));

    flashDevice->dtp.command = command;

    int deviceStatus = SYSCALL(IOWAIT, FLASHINT, deviceNumber, FALSE);

    setSTATUS(getSTATUS() | IECON);

    // Release the mutex
    SYSCALL(VERHOGEN, (memaddr) &deviceSemaphores[semNum], 0, 0);

    if (deviceStatus != 1) {
        // Release the mutex on the swap pool semaphore
        SYSCALL(VERHOGEN, (memaddr) &swapPoolSemaphore, 0, 0);

        //Raise a trap and kill it if something doesn't work
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
    //Controllo la cause della tlb exception, se è una TLB-Modification genero una trap
    if((support->sup_exceptState[PGFAULTEXCEPT].cause & GETEXECCODE) >> CAUSESHIFT == 1){
        generalExceptionHandler(support);
    }

    SYSCALL(PASSEREN, (memaddr) &swapPoolSemaphore, 0, 0);

    //Get missing page number
    unsigned int pageNum = GETVPN(support->sup_exceptState[PGFAULTEXCEPT].entry_hi);;
    unsigned int id = support->sup_asid;

    //Seleziono un frame con il replacement algorithm
    int i = replacementAlgorithm();

    //If it is occupied get its informations
    if(swapTable[i].sw_asid != -1){
        //Numero pagina dell'occupante
        int occupiedPageNum = swapTable[i].sw_pageNo;
        //ASID processo occupante
        int occupiedId = swapTable[i].sw_asid;

        //Disables the interrupts.
        setSTATUS(getSTATUS() & (~IECON));

        // Marco la entry come non valida
        pteEntry_t *occupiedEntry = swapTable[i].sw_pte;
        occupiedEntry->pte_entryLO &= ~VALIDON;
        // Aggiorno il TLB se serve
        updateTLB(occupiedEntry);

        //Restores the previous status.
        setSTATUS(getSTATUS() | IECON);

        // Update process x's backing store
        if(occupiedEntry->pte_entryLO & DIRTYON){
            writeFlash(occupiedId-1, occupiedPageNum, i, support);
        }
    }

        // Read the contents from the flash device
        readFlash(id-1, pageNum, i, support);

        setSTATUS(getSTATUS() & (~IECON));

        swapTable[i].sw_asid = id;
        swapTable[i].sw_pageNo = pageNum;
        swapTable[i].sw_pte = &(support->sup_privatePgTbl[pageNum]);

        // Update the process' page table
        unsigned int pageAddress = POOLSTART + (i * PAGESIZE);
        support->sup_privatePgTbl[pageNum].pte_entryLO = pageAddress | VALIDON | DIRTYON;

        // Update the TLB
        updateTLB(&(support->sup_privatePgTbl[i]));

        //Restores the previous status.
        setSTATUS(getSTATUS() | IECON);

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