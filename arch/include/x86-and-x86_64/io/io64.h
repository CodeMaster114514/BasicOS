#ifndef IO64_H
#define IO64_H

#include "io32.h"

static inline UINT64 io_in_cr3_64()
{
    UINT64 cr3;
    asm(
        "mov rax, cr3\n\t"
        "mov %0, rax\n\t"
        :"=m"(cr3)
        :
        :"rax"
    );
    return cr3;
}

static inline void io_out_cr3_64(UINT64 cr3)
{
    asm(
        "mov rax, %0\n\t"
        "mov cr3, rax\n\t"
        :
        :"m"(cr3)
        :"rax"
    );
}

#endif