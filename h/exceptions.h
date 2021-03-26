#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <umps3/umps/libumps.h>
#include <memory.h>
#include <pcb.h>
#include <asl.h>

extern pcb_t* currentProcess;

void passUpOrDie(unsigned index);
unsigned exceptionType();
void exceptionHandler();

#endif