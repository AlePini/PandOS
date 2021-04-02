#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <pandos_const.h>
#include <pandos_types.h>
#include <pcb.h>
#include <asl.h>
#include <scheduler.h>
#include <exceptions.h>
#include <umps3/umps/libumps.h>

void sysHandler();

/* SYSCALLS */
void createProcess(state_t*, support_t*);
void terminateProcess();
void passeren(int*);
pcb_t* verhogen(int*);
void waitIO(int, int, int);
void getCpuTime();
void waitForClock();
void getSupportStruct();

#endif