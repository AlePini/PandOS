#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#define CAUSE_IP_GET(cause,line) (cause & CAUSE_IP_MASK) & CAUSE_IP(line)

/**
 * @brief Handler for the interrupts. It gets called by the exception handler.
 */
void interruptHandler();


#endif