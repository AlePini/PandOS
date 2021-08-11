/****************************************************************************
 *
 * DELTE THIS COMMENT WHEN YOU'RE DONE
 *
 * This module implements : 
 * [ ] General exceptions handler
 * [ ] SYSCALL exceptions handler
 * [ ] Program Trap exceptions handler
 *
 ****************************************************************************/

#ifndef SYSSUPPORT_H
#define SYSSUPPORT_H

#include <pandos_types.h>

void generalExceptionHandler();

void syscallExceptionHandler(int sysNumber, support_t *support);

void program(support_t *support);

void terminate(support_t *support);

void getTOD(support_t *support);

void writePrinter(char* virtAddr, int len, support_t* support);

void writeTerminal(char *virtAddr, int len, support_t* support);

void readTerminal(char *buffer, support_t *support);

#endif