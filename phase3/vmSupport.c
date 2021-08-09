#include <umps3/umps/arch.h>
#include <umps3/umps/libumps.h>

#include <vmSupport.h>
#include <syscalls.h>
#include <initProc.h>
#include <sysSupport.h>

#define HEADERPAGE 0
#define HEADERTEXTSIZE 0x0014
#define HEADERDATASIZE 0x0014

#define POOLSTART (RAMSTART + (32 * PAGESIZE))
#define PAGESHIFT 12

#define NOTUSED -1
#define GETVPN(X) ((UPROCSTACKSTART <= X && X <= USERSTACKTOP) ?  (MAXPAGES-1) : ((X - VPNSTART) >> VPNSHIFT))
//Forse della roba va volatile
memaddr* swapPool;
memaddr swapPoolEntries[POOLSIZE];
HIDDEN swap_t swapTable[POOLSIZE];
HIDDEN SEMAPHORE swapPoolSemaphore;
int dataPages[UPROCNUMBER];

void initSwapStructs(){


    for (int i = 0; i < SUPP_SEM_NUMBER; i++){
        for (int j = 0; j < UPROCMAX; j++){
            semMutexDevices[i][j] = 1;
        }
    }

    //Per dire che un frame non è occupato si mette l'ASID a negativo dato che tutti i veri frame avranno ASID >0
    //Inizialization of swap table
    for(int i=0; i<POOLSIZE; i++){
        swapTable[i].sw_asid = NOTUSED;
    }

    //Initialization of the swap pool semaphore
    swapPoolSemaphore = 1;
    #TODO: master semaphore
    #TODO: roba sul supporto
}

int replacementAlgorithm() {

    //Static so it stores the value of the last frame used
    static int globalIndex = 0;
    int i = 0;

    //Finchè li trovo pieni e non ho gia fatto un giro completo di tutti i frame
    while((swapTable[(globalIndex + i) % POOLSIZE].sw_pte->pte_entryLO & VALIDON) && (i < POOLSIZE))
        i++;

    if(i == POOLSIZE) i = 1;

    globalIndex = (globalIndex + i) % POOLSIZE

    return globalIndex;
}

void updateTLB(pteEntry_t *newEntry){

    // Check if the new TLB entry is cached in the current TLB
    setENTRYHI(newEntry->pte_entryHI);
    TLBP();

    if ((getINDEX() & PRESENTFLAG) == 0) {
        // Update the TLB
        setENTRYLO(newEntry->pte_entryLO);
        TLBWI();
    }
}

bool checkMutexBusy(){
    return swapPoolSemaphore <= 0;
}

void PSV(){
    SYSCALL(VERHOGEN, &swapPoolSemaphore, 0, 0)
}

bool checkOccupied(int i){
    return swapTable[i].sw_pte->pte_entryLO & VALIDON
}

void updateTLB(pteEntry_t *newEntry){

    // Check if the updated TLB entry is cached in the TLB
    setENTRYHI(newEntry->pte_entryHI);
    TLBP();

    if ((getINDEX() & PRESENTFLAG) == 0) {
        // Update the TLB
        setENTRYLO(updatedEntry->pte_entryLO);
        TLBWI();
    }
}


void executeFlashAction(int deviceNumber, unsigned int primaryPage, unsigned int command, support_t *currentSupport) {
    // Obtain the mutex on the device
    memaddr primaryAddress = (primaryPage << PFNSHIFT) + POOLSTART;
    SYSCALL(PASSEREN, (memaddr) &semMutexDevices[FLASHSEM][deviceNumber], 0, 0);
    dtpreg_t *flashRegister = (dtpreg_t *) DEV_REG_ADDR(FLASHINT, deviceNumber);
    flashRegister->data0 = primaryAddress;
    flashRegister->data0 = primaryAddress;

    // Disabling interrupt doesn't interfere with SYS5, since SYSCALLS aren't
    // interrupts
    setSTATUS(getSTATUS() & (~IECON));;

    flashRegister->command = command;

    // Wait for the device
    // Note: the device ACK is handled by SYS5
    unsigned int deviceStatus = SYSCALL(IOWAIT, FLASHINT, deviceNumber, FALSE);

    setSTATUS(getSTATUS() | IECON);;

    // Release the mutex
    SYSCALL(VERHOGEN, (memaddr) &semMutexDevices[FLASHSEM][deviceNumber], 0, 0);

    if (deviceStatus != READY) {
        // Release the mutex on the swap pool semaphore
        SYSCALL(VERHOGEN, (memaddr) &semSwapPool, 0, 0);

        // Raise a trap
        trapExceptionHandler(currentSupport);
    }
}


void readFlash(int devNumber, unsigned int blockIndex, unsigned int pageIndex, support_t *support) {
    unsigned int action = FLASHREAD | (blockIndex << 8);
    executeFlashAction(devNumber, pageIndex, action, support);
}


void writeFlash(int devNumber, unsigned int blockIndex, unsigned int pageIndex, support_t *support) {
    unsigned int action = FLASHWRITE | (blockIndex << 8);
    executeFlashAction(devNumber, pageIndex, action, support);
}

void pager(){

    support_t *support = (support_t *) SYSCALL(GETSUPPORTPTR, 0, 0, 0);
    state_t currProcState = support->sup_exceptState[PGFAULTEXCEPT];
    //Controllo la cause della tlb exception, se è una TLB-Modification genero una trap
    if((currProcState.cause & GETEXECCODE) >> CAUSESHIFT == 1){
        programTrapExceptionHandler(support);
    }

    SYSCALL(PASSEREN, (memaddr) &swapPoolSemaphore, 0, 0);

    //Get missing page number
    unsigned int p = GETVPN(support->sup_exceptState[PGFAULTEXCEPT].entry_hi);;
    unsigned int a = support->sup_asid;

    //Seleziono un frame con il replacement algorithm
    int i = replacementAlgorithm();

    //Check if that entry is occupied
    bool occupied = checkOccupied(i);

    //If it is occupied get its informations
    if(occupied){
        //Numero pagina dell'occupante
        int k = swapTable[i].sw_pageNo;
        //ASID processo occupante
        int x = swapTable[i].sw_asid;

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
            writeFlash(x-1, k, i, support);
        }
    }

        // Read the contents from the flash device
        readFlash(a-1, p, i, support);

        setSTATUS(getSTATUS() & (~IECON));

        swapTable[i].sw_asid = a;
        swapTable[i].sw_pageNo = p;
        swapTable[i].sw_pte = &(support->sup_privatePgTbl[p]);

        // Update the process' page table
        support->sup_privatePgTbl[p].pte_entryLO |= VALIDON;
        SETPFN(support->sup_privatePgTbl[p].pte_entryLO, i);

        // Update the TLB
        updateTLB(&(support->sup_privatePgTbl[i]));

        //Restores the previous status.
        setSTATUS(getSTATUS() | IECON);

        SYSCALL(VERHOGEN, (memaddr) &swapPoolSemaphore, 0, 0);

        LDST((state_t *) &(support->sup_exceptState[PGFAULTEXCEPT]));
}


void uTLB_RefillHandler() {

    volatile unsigned int pageNum;
    pageNum = GETVPN(EXCSTATE->entry_hi);

    pteEntry_t *newEntry = &(currentProcess->p_supportStruct->sup_privatePgTbl[pageNum]);

    setENTRYHI(entry->pte_entryHI);
    setENTRYLO(entry->pte_entryLO);
    TLBWR();

    if (currentProcess == NULL)
		schedule();
	else
		LDST(EXCEPTION_STATE);
}