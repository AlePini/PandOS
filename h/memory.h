#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

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