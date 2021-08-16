#include <memory.h>

void mem_break(){
    return;
}

void *memcpy(void *dest, const void *src, size_t n){
    char *d = dest;
    const char *s = src;
    while (n--)
       *d++ = *s++;
    mem_break();
    return dest;  
}
