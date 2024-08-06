#ifndef MEMORY_H
#define MEMORY_H

#include "config.h"
#include "btype.h"

typedef unsigned long int size_t;

#if X86_64

#include "x86-and-x86_64/memory.h"

#endif
typedef struct
{
	MMAP *mmap;
	UINT64 MmapCount;
	MMAP *nextWrite;
	bool isFull;
	bool hasChanged;
} MemoryConfigure;

void InitMemory(MMAP *mmap, UINT64 count, int LinearAddrSizd, int PhysicalAddrSize);
void *alloc(size_t size);
MMAP *FindAFreeMMAP();
MMAP *FindThePreviousMMAPOf(MMAP *mmap);
int HowManyCanPut();
void *allocA4KBPage(UINT8 type, UINT8 flags);

#endif