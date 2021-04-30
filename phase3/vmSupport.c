#include <vmSupport.h>
#include <syscalls.h>

//Forse della roba va volatile

//Inizialization of swap pool
memaddr* swapPool = (memaddr) &SWAPPOOLSTART;
memaddr swapPoolEntries[POOLSIZE];
memcpy(swapPool, swapPoolEntries, sizeof(inputs));

//Per dire che un frame non è occupato si mette l'ASID a negativo dato che tutti i veri frame avranno ASID >0
//Inizialization of swap table
swap_t swapTable[POOLSIZE];
for(int i=0; i<POOLSIZE; i++){
    swapTable[i].sw_asid = -1;
}

//Initialization of the swap pool semaphore

SEMAPHORE swapPoolSemaphore = 1;


void pager(){
    
}