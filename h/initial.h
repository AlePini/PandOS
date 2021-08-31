#ifndef MAIN_H
#define MAIN_H

#include <pandos_types.h>
#include "umps3/umps/libumps.h"


#define SEM_NUM 48  //numero di semafori da mantenere

/**
 * @brief Counts active processes.
 */
extern unsigned int processCount;

/**
 * @brief Counts blocked processes on device semaphores.
 */
extern unsigned int softBlockCount;

/**
 * @brief Queue of the processes in the
 * running state.
 */
extern pcb_t *readyQueue;

/**
 * @brief The active process.
 */
extern pcb_t *currentProcess;

/**
 * @brief Device semaphores.
 * 
 */
extern SEMAPHORE semaphoreList[SEM_NUM];

/**
 * @brief Semaphore for the System Wide Interval Timer.
 * 
 */
extern SEMAPHORE swiSemaphore;


#endif