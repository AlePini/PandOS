#ifndef SCHEDULER_H
#define SCHEDULER_H

/**
 * @brief Picks the first process from the ready queue
 * and and executes it. Before of doins so inserts the 
 * current one back in the ready queue.
 */
void scheduler();


#endif