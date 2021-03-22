#include <syscall.h>
#include <initial.h>
#include <utils.h>




void Terminate_process(pcb_t* p){
    if(p == NULL) return;
    if(!emptyChild(p)) Terminate_process(p -> p_child);
    if(p -> p_next_sib != NULL) Terminate_process(p -> p_next_sib);
    outChild(p);
    outProcQ(getReadyQueue(), p);
    /* Da fare verifica sul semaforo se è negativo e non device bisogna aggiustarne il valore */
    outBlocked(p);
    freePcb(p); 
}

void passeren(int* semaddr){
    (*semaddr)--;
    if(*semaddr <= 0){
        insertBlocked(semaddr, currentProcess);
        //manca gestione del time
        currentProcess=NULL;
    }
    return;
}

void verhogen(int* semaddr){
    (*semaddr)++;
    if(*semaddr <= 0){
        pcb_t* tmp = removeBlocked(semaddr);
        insertProcQ(getReadyQueue(),tmp);
    }
    return;
}
