#ifndef SYSCALL_H
#define SYSCALL_H

#include <initial.h>
#include <scheduler.h>
#include <umps3/umps/libumps.h>
#include <exceptions.h>

void sysHandler();

/* SYSCALLS */
void createProcess();
void terminateProcess();
void passeren(int*);
void verhogen(int*);
void waitIO(int, int, int);
void getCPUTime();
void waitForClock();
void getSupportData();

#endif