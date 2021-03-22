#ifndef SYSCALL_H
#define SYSCALL_H

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