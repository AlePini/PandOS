#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

/**
 * @file    Exceptions
 * @author  Juri Fabbri, Alessandro Filippini, Filippo Bertozzi, Leonardo Giacomini
 * @brief   Implements exception handler who manages the other handlers
 * @version 0.2
 * @date    2021-04-03
 */

/**
 * @brief this function handles both TLB Exceptions and all others types of trap
 * What type of exception is handled depends on the input passed and
 * the function behaviour is related to the current process support
 * structure value, that can exist or be NULL.
 *
 * @param index 0 (PGFAULTEXCEPT) or 1 (GENERALEXCEPT), indicating
 * the type of the exception to be handled
 */
HIDDEN void passUpOrDie(unsigned int index)

/**
 * @brief Handles a TLB exception.
 *
 */
void TLBExcHandler();

/**
 * @brief Handles a Program Trap.
 *
 */
void generalTrapHandler();

/**
 * @brief Handles exceptions passing the to their custom handler.
 */
void exceptionHandler();

#endif