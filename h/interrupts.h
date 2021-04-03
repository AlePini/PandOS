#ifndef INTERRUPTS_H
#define INTERRUPTS_H

/**
 * @file    Interrupts
 * @author  Juri Fabbri, Alessandro Filippini, Filippo Bertozzi, Leonardo Giacomini
 * @brief   Implements interrupt handler who manages interrupts.
 * @version 0.2
 * @date 2021-04-03
 */

#define CAUSE_IP_GET(cause,line) (cause & CAUSE_IP_MASK) & CAUSE_IP(line)

/**
 * @brief Handler for the interrupts. It gets called by the exception handler.
 */
void interruptHandler();


#endif