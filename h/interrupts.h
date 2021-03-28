#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <pandos_const.h>
#include <pandos_types.h>
#include <umps3/umps/cp0.h>
#include <umps3/umps/types.h>
#include <umps3/umps/arch.h>

void interruptHandler();
void PLTInterrupt();
void terminalHandler();
void dtpHandler(int);
int getDeviceNr(unsigned);

#endif