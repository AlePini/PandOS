#ifndef INITPROC_H
#define INITPROC_H

#define VPNSTART 0x80000000
#define UPROCSTACKSTART 0xBFFFF000

void initSwapStructs();
void initializeSemaphores();
void initializeProcesses();
void handleMasterSemaphore();

/**
 * @brief First function that will be called by the phase 2.
 */
void test();

#endif