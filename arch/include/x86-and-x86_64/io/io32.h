#ifndef IO32_H
#define IO32_H

#include "btype.h"

inline UINT8 io_in8(UINT16 port)
{
    UINT8 data
    asm(
        "push edx\n\t"
        "push eax\n\t"
        "mov dx, %1\n\t"
        "in al, dx\n\t"
        "mov %0, al\n\t"
        "pop eax\n\t"
        "pop edx\n\t"
        :
        :
    );
}
inline void io_out8(UINT16 port, UINT8);

#endif