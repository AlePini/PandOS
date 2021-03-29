#include <pandos_const.h>
#include <pandos_types.h>
#include <pcb.h>
#include <asl.h>
#include <scheduler.h>
#include <exceptions.h>
#include <umps3/umps/libumps.h>
#include <initial.h>

//TODO: mettere HIDDEN tutto quello che viene usato solo nel suo file


//Prova a mettere la roba extern qui
extern void test();
extern void uTLB_RefillHandler();
extern void exceptionHandler();


int main(){

    //Inizializzare strutture dati livello 2
    initPcbs();
    initASL();

    //Popolare il passup vector
    passupvector_t* passup = (passupvector_t*)PASSUPVECTOR;
    passup->tlb_refill_handler = (memaddr) &uTLB_RefillHandler;
    passup->tlb_refill_stackPtr = (memaddr) KERNELSTACK;
    passup->exception_handler = (memaddr) exceptionHandler;
    passup->exception_stackPtr = (memaddr) KERNELSTACK;


    // //Inizializzare le variabili globali
    readyQueue = mkEmptyProcQ();
    semIntTimer = 0;
    currentProcess = NULL;
    //processCount, softBlockCount and device semaphores si inizializzano da soli ai valori di default


    //Setup del system-wide timer
    LDIT(SWTIMER);

    //Parte sul primo processo
    pcb_t* firstProcess = allocPcb();
    processCount++;

    //Setup dello status
    firstProcess->p_s.status = ALLOFF | TEBITON | IEPON | IMON;

    //SP is set to RAMTOP
    RAMTOP(firstProcess->p_s.reg_sp);
    //Program Counter set to test address
    firstProcess->p_s.pc_epc = (memaddr) test;
    firstProcess->p_s.reg_t9 = (memaddr) test;
    insertProcQ(&readyQueue, firstProcess);

    currentProcess = NULL;

    //Scheduler
    scheduler();

    return 0;
}
