#ifndef MEMORY_H
#define MEMORY_H

/**
 * @file    Memory
 * @author  Juri Fabbri, Alessandro Filippini, Filippo Bertozzi, Leonardo Giacomini
 * @brief   Implements the memcopy function.
 * @version 0.2
 * @date 2021-04-03
 */

/**
 * @brief Copies bytes from an address to
 * another.
 * 
 * @param dest Destination.
 * @param src Source.
 * @param len Length of the bytes to be copied.
 * @return void* A pointer to the destination
 * address.
 */
void *memcpy(void *dest, const void *src, size_t n);

#endif