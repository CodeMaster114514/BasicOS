#ifndef MEMORY_H
#define MEMORY_H

#include "config.h"
#include "btype.h"

typedef unsigned long int size_t;

#if X86_64

#include "x86-and-x86_64/memory.h"

#endif

void InitMemory(MMAP *mmap, UINT64 count);
void *alloc(size_t size);

#endif