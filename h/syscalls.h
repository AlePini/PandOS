#ifndef SYSCALLS_H
#define SYSCALLS_H

/**
 * @file    Syscall
 * @author  Juri Fabbri, Alessandro Filippini, Filippo Bertozzi, Leonardo Giacomini
 * @brief   Implements syscall handler who manages syscalls.
 * @version 0.2
 * @date    2021-04-03
 */

#include <pandos_types.h>
#include <pandos_const.h>

/**
 * @brief  Handler for the syscalls. It gets called by the exception handler.
 */
void sysHandler();

/* SYSCALLS */

/**
 * @brief SYS1: creates a new process with the state and the support structure passed as parameters.
 * Returns an exit code who specifies if the operation was completed succesfully.
 * 
 * @param statep State of the new process.
 * @param supportp Support structure of the new process.
 * 
 */
void createProcess(state_t * statep, support_t * supportp);

/**
 * @brief  SYS2: terminate the running process and all it's progeny recursively.
 */
void terminateProcess();

/**
 * @brief SYS3 (P): Does a P operation on the semaphore passed as parameter.
 * 
 * @param semAdd Pointer to the semaphore to perform the P on.
 */
void passeren(int *semAdd);

/**
 * @brief SYS4 (V): Does a V operation on the semaphore passed as parameter.
 * 
 * @param semAdd Pointer to the semaphore to perform the V on.
 * @return The pointer to the PCB that was unblocked from the V, otherwise returns NULL.
 */
pcb_t* verhogen(int *semAdd);

/**
 * @brief SYS5: waits for an I/O operation. It blocks the current process on a (sub)device
 * semaphore specified by the parameteres.
 * 
 * @param intlNo Interrupt line.
 * @param dNum  Device number of that line.
 * @param waitForTermRead Specifies if the terminal reads or writes.
 */
void waitIO(int intlNo, int dNum, bool waitForTermRead);

/**
 * @brief SYS6: returns the total time a process
 * has been executed, sotring the value in the v0 register.
 */
void getCpuTime();

/**
 * @brief SYS7: blocks the current process in the
 * System wide interval semaphore until the next SW interrupt.
 */
void waitForClock();

/**
 * @brief SYS8: returns the pointer to the currentProcess'
 * support struct, saving it in the register v0.
 */
void getSupportStruct();

#endif