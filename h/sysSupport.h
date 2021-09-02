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