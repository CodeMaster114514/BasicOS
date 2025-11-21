#ifndef TASK_H
#define TASK_H

#include "btype.h"

typedef struct TSS
{
    #if X86_64
    UINT32 reserved1;
    UINT64 rsp0;
    UINT64 rsp1;
    UINT64 rsp2;
    UINT64 reserved2;
    UINT64 ist1;
    UINT64 ist2;
    UINT64 ist3;
    UINT64 reserved3;
    UINT16 reserved4;
    UINT16 iomapBaseAddress;
    #endif
} __attribute__((packed)) TSS;

typedef struct TSSDescriptor
{
    UINT16 limit;
    UINT64 base;
} __attribute__((packed)) TSSDescriptor;

#endif