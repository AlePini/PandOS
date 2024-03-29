#ifndef SCHEDULER_H
#define SCHEDULER_H

/**
 * @file    Scheduler
 * @author  Juri Fabbri, Alessandro Filippini, Filippo Bertozzi, Leonardo Giacomini
 * @brief   Implements the scheduler that handles the process queue.
 * @version 0.2
 * @date    2021-04-03
 */

#include <pandos_types.h>

/**
 * @brief Picks the first process from the ready queue
 * and and executes it. Before of doing so inserts the
 * current one back in the ready queue.
 */
void scheduler();

/**
 * @brief Gets the time elapsed as current process.
 *
 * @return time past from the moment the current process has been selected
 */
cpu_t getTimeSlice();

#endif