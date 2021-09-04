#ifndef VMSUPPORT_H
#define VMSUPPORT_H

/**
 * @file    vmSupport
 * @author  Juri Fabbri, Alessandro Filippini, Filippo Bertozzi, Leonardo Giacomini
 * @brief   Implements TLB exception handler and functions for read/write flash devices.
 * @version 0.3
 * @date    2021-09-01
 */

#include <pandos_types.h>

/**
 * @brief Initializes third level structs.
 */
void initSwapStructs();

/**
 * @brief Clears the swap table.
 */
void clearSwap(int asid);

/**
 * @brief Selects a new frame where to write.
 */
int replacementAlgorithm();

/**
 * @brief Updates the TLB.
 *
 * @param newEntry Pointer to the new entry in TLB.
 */
void updateTLB(pteEntry_t *newEntry);

/**
 * @brief Reads or writes a flash device.
 *
 * @param deviceNumber Device index.
 * @param primaryPage Page index in primary memory.
 * @param command Command to be used.
 * @param currentSupport Pointer to the support structure of the current process.
 *
 */
void executeFlashAction(int deviceNumber, unsigned int pageIndex, unsigned int command, support_t *support);

void readFlash(int deviceNumber, unsigned int blockIndex, unsigned int pageIndex, support_t *support);

void writeFlash(int deviceNumber, unsigned int blockIndex, unsigned int pageIndex, support_t *support);

/**
 * @brief Handles TLB Page Fault exceptions.
 */
void pager();

/**
 * @brief Handles TLB Refill exceptions.
 */
void uTLB_RefillHandler();


#endif