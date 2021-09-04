#ifndef SYSSUPPORT_H
#define SYSSUPPORT_H

/**
 * @file    SysSupport
 * @author  Juri Fabbri, Alessandro Filippini, Filippo Bertozzi, Leonardo Giacomini
 * @brief   Implements general, syscall and program trap handlers.
 * @version 0.3
 * @date    2021-09-01
 */

#include <pandos_types.h>

#define TEXT_AND_DATA_TOP 0x8001E000
#define ADDRESS_IN_RANGE(S, E) ( ( (S >= (char *) KUSEG) && (E <= (char *) TEXT_AND_DATA_TOP) ) || ( (S >= (char *) UPROCSTACKSTART) && (E <= (char *) USERSTACKTOP) ) )
#define GET_DEVICE_NUMBER(S) S->sup_asid - 1

extern int deviceSemaphores[SEMNUM];

/**
 * @brief Handles the third level exceptions.
 */
void generalExceptionHandler();

/**
 * @brief Handles the syscalls not handled by the level 2
 *
 * @param sysNumber Number of the syscall who generated the exception
 *
 * @param support Pointer at the support structure of the process that caused the exception.
 *
 */
void syscallExceptionHandler(int sysNumber, support_t *support);

/**
 * @brief Trap Exceptions Handler.
 *
 * @param support Pointer at the support structure of the process that caused the exception
 */
void programTrapExceptionHandler(support_t *support);

/**
 * @brief Terminate a process, wrapper of the level 2 function with the same goal.
 *
 * @param currentSupport Pointer to the support structure of the
 * current process.
 */
void terminate(support_t *support);

/**
 * @brief Stores the TOD in the current process v0 register.
 *
 */
void getTOD(support_t *support);

/**
 * @brief Writes a string to the printer.
 *
 * @param string Pointer to the first character of the string.
 * @param len Lenght of the string.
 * @param support Pointer to the support structure of the current process.
 */
void writePrinter(char* string, int len, support_t* support);

/**
 * @brief Writes a string to the terminal.
 *
 * @param string Pointer to the first character of the string.
 * @param len Lenght of the string.
 * @param support Pointer to the support structure of the current process.
 */
void writeTerminal(char *string, int len, support_t* support);

/**
 * @brief Reads a string from the terminal used by the current process.
 *
 * @param string Pointer to the first character that will store the string.
 * @param support Pointer to the support structure of the current process.
 *
 */
void readTerminal(char *string, support_t *support);

#endif