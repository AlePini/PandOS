#include <initial.h>
#include <pandos_const.h>
#include <pandos_types.h>
#include <pcb.h>
#include <asl.h>
#include <scheduler.h>

/*Dichiarazione variabii globali*/
unsigned int processCount;          //process count
unsigned int softBlockCount;         //soft-block count
pcb_PTR readyQueue;      //ready queue
pcb_PTR currentProcess;    //current process
int semaphoreList[SEM_NUM]; //device semaphores
int swiSemaphore;

/*Funzioni extern*/
extern void exceptionHandler();
extern void test();
extern void uTLB_RefillHandler();
extern void copyState();

int main()
{

    //Initialize data structure level 2.
    initPcbs();
    initASL();

    //Populates the passup vector.
    passupvector_t* passup = (passupvector_t*)PASSUPVECTOR;
    passup->tlb_refill_handler = (memaddr) &uTLB_RefillHandler;
    passup->tlb_refill_stackPtr = (memaddr) KERNELSTACK;
    passup->exception_handler = (memaddr) exceptionHandler;
    passup->exception_stackPtr = (memaddr) KERNELSTACK;

    //Initializes global variables.
    readyQueue = mkEmptyProcQ();
    currentProcess = NULL;
    processCount = 0;
    softBlockCount = 0;
    swiSemaphore = 0;
    for (int i=0; i<DEVICE_NUMBER; i++){
        semaphoreList[i] = 0;
    }

    //Setup del system-wide timer.
    LDIT(SWTIMER);

    //Creation of first process.
    pcb_t* firstProcess = allocPcb();
    processCount++;

    //Setup of status.
    firstProcess->p_s.status = ALLOFF | TEBITON | IEPON | IMON;
    //SP is set to RAMTOP.
    RAMTOP(firstProcess->p_s.reg_sp);
    //Program Counter set to test address.
    firstProcess->p_s.pc_epc = (memaddr) test;
    firstProcess->p_s.reg_t9 = (memaddr) test;
    insertProcQ(&readyQueue, firstProcess);

    //Call of the scheduler.
    scheduler();

    return 0;
}