#include <vmSupport.h>
#include <syscalls.h>

//Forse della roba va volatile
memaddr* swapPool;
memaddr swapPoolEntries[POOLSIZE];
swap_t swapTable[POOLSIZE];
SEMAPHORE swapPoolSemaphore;

void initSwapStructs(){
 
    //Inizialization of swap pool
    swapPool = (memaddr) &SWAPPOOLSTART;
    memcpy(swapPool, swapPoolEntries, sizeof(inputs));

    //Per dire che un frame non è occupato si mette l'ASID a negativo dato che tutti i veri frame avranno ASID >0
    //Inizialization of swap table
    for(int i=0; i<POOLSIZE; i++){
        swapTable[i].sw_asid = -1;
    }

    //Initialization of the swap pool semaphore
    swapPoolSemaphore = 1;
}

void pager(){

    SYSCALL(GETSUPPORTPTR, 0, 0, 0);
    // Non mi serve tutta la support, mi basta lo state
    state_t currProcState = EXCEPTION_STATE->reg_v0->sup exceptState[0];

    int tlbnum = currProcState->s_cause;
    if(tlbnum == TLBINVLDL || tlbnum == TLBINVLDS){

        SYSCALL(PASSEREN, &swapPoolSemaphore, 0, 0);

        //Get missing page number
        unsigned int p = currProcState->s_entryHI>>VPNSHIFT;

        //Selezionare il frame i con Algoritmo FIFO (Section 4.5.4)

    } else if(tlbnum == TLBMOD){
        //Call program trap exception handler
    } else {
        // Invalid and should never happnes
    }
}
