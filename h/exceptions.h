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