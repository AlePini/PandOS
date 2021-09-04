#ifndef INITPROC_H
#define INITPROC_H

/**
 * @file    initProc
 * @author  Juri Fabbri, Alessandro Filippini, Filippo Bertozzi, Leonardo Giacomini
 * @brief   Implements test and export Support level global variables.
 * @version 0.3
 * @date    2021-09-01
 */

#define VPNSTART 0x80000000
#define UPROCSTACKSTART 0xBFFFF000

extern int masterSemaphore;

void initializeSemaphores();
void initializeProcesses();
void handleMasterSemaphore();

/**
 * @brief First function that will be called by the phase 2.
 */
void test();

#endif