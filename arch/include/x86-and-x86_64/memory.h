#include "config.h"
#include "btype.h"

#ifndef ARCHMEMORY_H
#define ARCHMEMORY_H

typedef struct
{
	UINT32 base;
	#if PAEMode | X86_64
	UINT32 high;
	#endif
} PTE;

typedef struct
{
	UINT32 base;
	#if PAEMode | X86_64
	UINT32 high;
	#endif
} PDE;

#define P 0x0000000000000001
#define R_W 0x0000000000000002
#define U_S 0x0000000000000004
#define PWT 0x0000000000000008
#define PCD 0x0000000000000010
#define PATbit 0x0000000000000080
#define G 0x0000000000000100

#define PageSize4KB 0

#if PATMode

#define PageSize2MB 1
#define NX 0x8000000000000000
#define PS2MB 0x0000000000000080

#endif

#if X86_64

typedef struct
{
	UINT32 base;
	#if PAEMode | X86_64
	UINT32 high;
	#endif
} PDPTE;

typedef struct
{
	UINT32 base;
	#if PAEMode | X86_64
	UINT32 high;
	#endif
} PML4E;

#define PS1GB 0x0000000000000080
#define XD 0x8000000000000000

#define PageSize1GB 2
#endif
void *MapAPageInFreeVirtualAddress(UINT64 PhysicalAddress, int size, UINT64 flags);
void InitPaging(int LinearAddrSize, int PhysicalAddrSize);
#endif