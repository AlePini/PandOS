#include <vmSupport.h>
#include <syscalls.h>
#include <limbumps.h>

//Forse della roba va volatile
memaddr* swapPool;
memaddr swapPoolEntries[POOLSIZE];
swap_t swapTable[POOLSIZE];
SEMAPHORE swapPoolSemaphore;

void initSwapStructs(){

    //Inizialization of swap pool
    swapPool = (memaddr) &SWAPPOOLSTART;
    memcpy(swapPool, swapPoolEntries, sizeof(memaddr)); //TODO: Devo capire bene cosa mettere nel sizeof

    //Per dire che un frame non è occupato si mette l'ASID a negativo dato che tutti i veri frame avranno ASID >0
    //Inizialization of swap table
    for(int i=0; i<POOLSIZE; i++){
        swapTable[i].sw_asid = -1;
    }

    //Initialization of the swap pool semaphore
    swapPoolSemaphore = 1;
}

bool checkOccupied(int i){
    return swapTable[i].sw_asid == -1;
}

int PPRA(){
    static int i=-1;
    i++;
    return i%POOLSIZE;
}

void pager(){

    SYSCALL(GETSUPPORTPTR, 0, 0, 0);
    // Non mi serve tutta la support, mi basta lo state
    support_t support = EXCEPTION_STATE->reg_v0;
    state_t currProcState = support->sup_exceptState[0];

    int tlbnum = currProcState->s_cause;
    if(tlbnum == TLBINVLDL || tlbnum == TLBINVLDS){

        SYSCALL(PASSEREN, &swapPoolSemaphore, 0, 0);

        //Get missing page number
        unsigned int p = currProcState->s_entryHI>>VPNSHIFT;
        unsigned int a = currProcState->s_entryHI>>ASIDSHIFT

        //Selezionare il frame i con Algoritmo FIFO (Section 4.5.4)
        int i = PPRA();


        //Check if that entry is occupied
        bool occupied = checkOccupied[i];

        //If it is occupied get its informations
        if(occupied){
            int k = swapTable[i].sw_pageNo;
            int x = swapTable[i].sw_asid;

        //Get the status
        unsigned oldStatus = getSTATUS();
        //Disables the interrupts.
        setSTATUS(oldStatus & DISABLEINTS);

        //TODO: parte a e b

        //Restores the previous status.
        setSTATUS(oldStatus);

        //TODO: parte sui flash device (c)
        }

        //TODO: parte sui flash device(9)

        swapTable[i].sw_asid = a;
        swapTable[i].sw_pageNo = p;
        swapTable[i].sw_pte = &(support->sup_privatePgTbl[p])

        //Get the status
        unsigned oldStatus = getSTATUS();
        //Disables the interrupts.
        setSTATUS(oldStatus & DISABLEINTS);

        //TODO: sta parte sicuro è sbagliata va checkata
        unsigned int eLO = swapTable[i].sw_pte->s_entryLO
        eLO += 1<<9;
        eLO += i<<12;

        TLBCLR();

        //Restores the previous status.
        setSTATUS(oldStatus);

        SYSCALL(VERHOGEN, &swapPoolSemaphore, 0, 0);
        LDST(EXCEPTION_STATE);

    } else if(tlbnum == TLBMOD){
        //TODO: Call program trap exception handler
    } else {
        //TODO: metti syscall che termina la situa
    }
}
