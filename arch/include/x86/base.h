#ifndef BASE_H
#define BASE_H

#include "btype.h"

typedef struct GDTR {
    UINT16 limit;
    #if X86_64
    UINT64 base;
    #else
    UINT32 base;
    #endif
} __attribute__((packed)) GDTR;

#endif