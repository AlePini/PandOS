#ifndef INITIAL_H
#define INITIAL_H
#include <pandos_const.h>
#include <pandos_types.h>

#define SEMAPHORE int

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
extern semaphore semDevices[DEVICE_NUMBER];

/**
 * @brief Semaphore for the System Wide Interval Timer.
 * 
 */
extern semaphore semIntTimer;

/**
 * @brief PandOS entry point.
 * 
 * @return The exit code.
 */
int main(void);

#endif