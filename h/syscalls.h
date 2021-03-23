#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <initial.h>
#include <umps3/umps/libumps.h>

void sysHandler();

/* SYSCALLS */
void createProcess(state_t*, support_t*);
void terminateProcess();
void passeren(int*);
void verhogen(int*);
void waitIO(int, int, int);
void getCpuTime();
void waitForClock();
void getSupportStruct();

extern int semDisk[];
extern int semFlash[];
extern int semNetwork[];
extern int semPrinter[];
extern int semTerminalTrans[];
extern int semTerminalRecv[];
extern int semIntTimer;

#endif