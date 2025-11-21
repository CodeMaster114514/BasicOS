#ifndef HEAP_H
#define HEAP_H

#include "btype.h"

typedef struct
{
	UINT32 len;// in 4kb 1 2 4 ... 256 ... 512 ... 1024 ... 512 ... 256 ... 4 2 1 2; 
	void *target;
	bool status;
	bool withLast; // 为true则target无效
	bool withNext; // 为true则下一个Block与这个block为一个block
} BlockInfo;

typedef struct HI
{
	UINT8 type;
	UINT32 len;
	BlockInfo *blocks;
	UINT32 FreeSize;
	BlockInfo *FirstFree;
	void *HeapStart;
	struct HI *next_has_free;
} HeapInfo;

typedef struct H
{
	HeapInfo infos[4096 - sizeof(struct H *)];
	struct H *next;
} Heaps;

#endif