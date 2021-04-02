#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <pandos_const.h>
#include <pandos_types.h>
#include <umps3/umps/arch.h>
#include <umps3/umps/cp0.h>
#include <umps3/umps/libumps.h>
#include <umps3/umps/types.h>
#include <asl.h>
#include <syscalls.h>

#define CAUSE_IP_GET(cause,line) (cause & CAUSE_IP_MASK) & CAUSE_IP(line)

void interruptHandler();
int getDeviceNr(unsigned);
void dtpHandler(int);
void terminalHandler();
void PLTInterrupt();
void SWITInterrupt();

#endif