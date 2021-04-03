#include <memory.h>

typedef unsigned int size_t;

void *memcpy(void *dest, const void *src, size_t n){
    char *d = dest;
    const char *s = src;
    while (n--)
       *d++ = *s++;
    return dest;  
}
