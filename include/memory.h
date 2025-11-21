#ifndef MEMORY_H
#define MEMORY_H

#include "config.h"
#include "btype.h"

typedef unsigned long int size_t;
#define NULL 0

#if X86_64

#include "x86/memory.h"

#endif
typedef struct
{
	MMAP *mmap;
	UINT64 MmapCount;
	MMAP *nextWriteStart;
	MMAP *nextWrite;
	bool isFull;
	bool debug;
} MemoryConfigure;

void InitMemory(MMAP *mmap, UINT64 count, int LinearAddrSizd, int PhysicalAddrSize);
void *alloc(size_t size);
void addMMAPAfter(MMAP *mmap, MMAP *target);
void addMMAPBefore(MMAP *mmap, MMAP *target);
void *allocA4KBPage(UINT8 type, UINT8 flags);
void *allocA2MBPage(UINT8 type, UINT8 flags);
void cleanMemory(void *target_address, UINT64 size);

#endif