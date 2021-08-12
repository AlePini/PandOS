#ifndef INITPROC_H
#define INITPROC_H

#define VPNSTART 0x80000000
#define UPROCSTACKSTART 0xBFFFF000

void initSwapStructs();
void initializeSemaphores();
void initializeProcesses();
void handleMasterSemaphore();
void test();

#endif