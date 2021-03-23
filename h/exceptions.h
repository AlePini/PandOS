#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <initial.h>
#include <umps3/umps/libumps.h>
#include <memory.h>

extern pcb_t* currentProcess;

void passUpOrDie(unsigned index);
unsigned exceptionType();
void exceptionHandler();

#endif