#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

extern pcb_t* currentProcess;

/**
 * @brief Handles exceptions passing the to their custom handler.
 */
void exceptionHandler();

#endif