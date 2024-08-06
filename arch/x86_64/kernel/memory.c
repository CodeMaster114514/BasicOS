#include "x86-and-x86_64/memory.h"
#include "x86-and-x86_64/io/io64.h"
#include "memory.h"

extern MemoryConfigure configure;

MMAP MapCache = {0};

struct
{
	int LinearAddrSize, PhysicalAddrSize;
} AddrLength;

void InitPaging(int LinearAddrSize, int PhysicalAddrSize)
{
	AddrLength.LinearAddrSize = LinearAddrSize;
	AddrLength.PhysicalAddrSize = PhysicalAddrSize;
}

typedef struct
{
	void *address;
	union
	{
		PTE *pte;
		PDE *pde;
		PDPTE *pdpte;
		PML4E *pml4e;
	};
} returnValue;

static returnValue FindFreePT(PTE *ptes, void *address)
{
	for (int i = 0; i < 0x1000 / sizeof(PTE); ++i)
	{
		if (!(ptes[i].base & P))
		{
			return (returnValue){.address = (void *)((UINT64)address | ((UINT64)i << 12)), .pte = &ptes[i]};
		}
	}
	return (returnValue){0};
}

static PDE *FindExeistentPD(PDE *pdes, int start)
{
	for (int i = start; i < 0x1000 / sizeof(PDE); ++i)
	{
		if ((pdes[i].base & P) && !(pdes[i].base & PS2MB))
		{
			return &pdes[i];
		}
	}
	return 0;
}

static PDE *FindExeistent2MBPD(PDE *pdes, int start)
{
	for (int i = start; i < 0x1000 / sizeof(PDE); ++i)
	{
		if ((pdes[i].base & P) && (pdes[i].base & PS2MB))
		{
			return &pdes[i];
		}
	}
}

static PDE *FindFreePD(PDE *pdes)
{
	for (int i = 0; i < 0x1000 / sizeof(PDE); ++i)
	{
		if (!(pdes[i].base & P))
		{
			return &pdes[i];
		}
	}
	return 0;
}

static returnValue FindFreePTFromPD(PDE *pdes, void *address)
{
	int index = 0;
research:
	PDE *exeistent = FindExeistentPD(pdes, index);
	if (exeistent)
	{
		index = ((UINT64)exeistent & 0xfff) / sizeof(PDE);
		if (index >= 0x1000 / sizeof(PDE))
			return (returnValue){0};
		returnValue value = FindFreePT((PTE *)(((UINT64)pdes << 9) | (UINT64)(index << 12)), (void *)((UINT64)address | ((UINT64)index << (12 + 9))));
		if (value.pte)
		{
			return value;
		}
		else
		{
			++index;
			goto research;
		}
	}
	else
	{
		PDE *FreePD = FindFreePD(pdes);
		index = ((UINT64)FreePD & 0xfff) / sizeof(PDE);
		MMAP *FreeMmap = FindAFreeMMAP();
		FreePD->base = FreeMmap->PhysicalAddress | P | R_W;
		FreePD->high = (UINT32)(FreeMmap->PhysicalAddress >> 32);
		returnValue value;
		switch (HowManyCanPut())
		{
		case 0:
			value = FindFreePT((PTE *)(((UINT64)pdes << 9) | (UINT64)(index << 12)), (void *)((UINT64)address | ((UINT64)index << (12 + 9))));
			value.pte->base = (FreeMmap->PhysicalAddress + 0x1000) | P | R_W;
			value.pte->high = (XD >> 32) | (UINT32)((FreeMmap->PhysicalAddress + 0x1000) >> 32);
			configure.nextWrite = (MMAP *)value.address;
			FindThePreviousMMAPOf(FreeMmap)->next = configure.nextWrite;
			*configure.nextWrite = (MMAP){
				.type = MAP_TYPE_OSDATA,
				.flags = MAP_FLAG_READ | MAP_FLAG_WRITE,
				.NoOfPage = 1,
				.PhysicalAddress = FreeMmap->PhysicalAddress,
				.VirtualAddress = (void *)(((UINT64)pdes << X86 ? 10 : 9) | (UINT64)(index << 12)),
				.next = 0};
			configure.nextWrite[1] = configure.nextWrite[0];
			configure.nextWrite[1].next = FreeMmap;
			configure.nextWrite[1].PhysicalAddress += 0x1000;
			configure.nextWrite[1].VirtualAddress = value.address;
			configure.nextWrite += sizeof(MMAP);
			FreeMmap->PhysicalAddress += 0x2000;
			value = FindFreePT((PTE *)(((UINT64)pdes << 9) | (UINT64)(index << 12)), (void *)((UINT64)address | ((UINT64)index << (12 + 9))));
			break;
		case 1:
			value = FindFreePT((PTE *)(((UINT64)pdes << 9) | (UINT64)(index << 12)), (void *)((UINT64)address | ((UINT64)index << (12 + 9))));
			*configure.nextWrite = (MMAP){
				.type = MAP_TYPE_OSDATA,
				.flags = MAP_FLAG_READ | MAP_FLAG_WRITE,
				.NoOfPage = 1,
				.PhysicalAddress = FreeMmap->PhysicalAddress,
				.VirtualAddress = (void *)(((UINT64)pdes << 9) | (UINT64)(index << 12)),
				.next = FreeMmap};
			FindThePreviousMMAPOf(FreeMmap)->next = configure.nextWrite;
			value.pte->base = (UINT32)(FreeMmap->PhysicalAddress + 0x1000) | P | R_W;
			value.pte->high = XD >> 32 | (UINT32)((FreeMmap->PhysicalAddress + 0x1000) >> 32);
			configure.nextWrite = value.address;
			*configure.nextWrite = (MMAP){
				.type = MAP_TYPE_OSDATA,
				.flags = MAP_FLAG_READ | MAP_FLAG_WRITE,
				.NoOfPage = 1,
				.PhysicalAddress = FreeMmap->PhysicalAddress + 0x1000,
				.VirtualAddress = value.address,
				.next = FreeMmap};
			FreeMmap->PhysicalAddress += 0x2000;
			break;
		default:
			configure.nextWrite[0] = (MMAP){
				.type = MAP_TYPE_OSDATA,
				.flags = MAP_FLAG_READ | MAP_FLAG_WRITE,
				.NoOfPage = 1,
				.PhysicalAddress = FreeMmap->PhysicalAddress,
				.VirtualAddress = (void *)(((UINT64)pdes << 9) | (UINT64)(index << 12)),
				.next = 0};
			break;
		}
		value = FindFreePT((PTE *)(((UINT64)pdes << 9) | (UINT64)(index << 12)), (void *)((UINT64)address | ((UINT64)index << (12 + 9))));
		return value;
	}
}

static PDPTE *FindExeistentPDPT(PDPTE *pdptes, int start)
{
	for (int i = start; i < 0x1000 / sizeof(PDPTE); ++i)
	{
		if ((pdptes[i].base & P) && !(pdptes[i].base & PS1GB))
		{
			return &pdptes[i];
		}
	}
	return 0;
}

static PDPTE *FindExeistent1GBPDPT(PDPTE *pdptes, int start)
{
	for (int i = start; i < 0x1000 / sizeof(PDPTE); ++i)
	{
		if ((pdptes[i].base & P) && (pdptes[i].base & PS1GB))
		{
			return &pdptes[i];
		}
	}
	return 0;
}

static PDPTE *FindFreePDPT(PDPTE *pdptes)
{
	for (int i = 0; i < 0x1000 / sizeof(PDPTE); ++i)
	{
		if (!(pdptes[i].base & P))
		{
			return &pdptes[i];
		}
	}
	return 0;
}

static returnValue FindFreePTFromPDPT(PDPTE *pdptes, void *address)
{
	int index = 0;
research:
	PDPTE *exeistent = FindExeistentPDPT(pdptes, index);
	if (exeistent)
	{
		index = ((UINT64)exeistent & 0xfff) / sizeof(PDPTE);
		returnValue value = FindFreePTFromPD((PDE *)(((UINT64)pdptes << 9) | ((UINT64)index << 12)), (void *)((UINT64)address | ((UINT64)index << (12 + 9 + 9))));
		if (index >= 0x1000 / sizeof(PDPTE))
			return (returnValue){0};
		if (value.pte)
		{
			return value;
		}
		else
		{
			++index;
			goto research;
		}
	}
	else
	{
		PDPTE *FreePDPT = FindFreePDPT(pdptes);
		index = ((UINT64)FreePDPT & 0xfff) / sizeof(PDPTE);
		MMAP *FreeMmap = FindAFreeMMAP();
		FreePDPT->base = FreeMmap->PhysicalAddress | P | R_W;
		FreePDPT->high = (UINT32)(FreeMmap->PhysicalAddress >> 32);
		returnValue value;
		switch (HowManyCanPut())
		{
		case 0:
			UINT64 PhysicalAddress = FreeMmap->PhysicalAddress;
			MMAP *Previous = FindThePreviousMMAPOf(FreeMmap);
			FreeMmap->PhysicalAddress += 0x1000;
			value = FindFreePTFromPD((PDE *)(((UINT64)pdptes << 9) | ((UINT64)index << 12)), (void *)((UINT64)address | ((UINT64)index << (12 + 9 + 9))));
			*configure.nextWrite = (MMAP){
				.type = MAP_TYPE_OSDATA,
				.flags = MAP_FLAG_READ | MAP_FLAG_WRITE,
				.NoOfPage = 1,
				.PhysicalAddress = PhysicalAddress,
				.VirtualAddress = (void *)(((UINT64)address << 9) | (UINT64)index << 12),
				.next = Previous->next ? Previous->next : &Previous[1]};
			Previous->next = configure.nextWrite;
			configure.nextWrite += sizeof(MMAP);
			break;
		default:
			*configure.nextWrite = (MMAP){
				.type = MAP_TYPE_OSDATA,
				.flags = MAP_FLAG_READ | MAP_FLAG_WRITE,
				.NoOfPage = 1,
				.PhysicalAddress = FreeMmap->PhysicalAddress,
				.VirtualAddress = (void *)(((UINT64)address << 9) | (UINT64)index << 12),
				.next = FreeMmap};
			FindThePreviousMMAPOf(FreeMmap)->next = configure.nextWrite;
			FreeMmap->PhysicalAddress += 0x1000;
			configure.nextWrite += sizeof(MMAP);
			value = FindFreePTFromPD((PDE *)(((UINT64)pdptes << 9) | ((UINT64)index << 12)), (void *)((UINT64)address | ((UINT64)index << (12 + 9 + 9))));
			break;
		}
		return value;
	}
}

static PML4E *FindExeistentPML4(PML4E *pml4es, int start)
{
	for (int i = start; i < 0x1000 / sizeof(PML4E); ++i)
	{
		if (pml4es[i].base & P)
		{
			return &pml4es[i];
		}
	}
	return 0;
}

static PML4E *FindFreePML4(PML4E *pml4es)
{
	for (int i = 0; i < 0x1000 / sizeof(PML4E); ++i)
	{
		if (!(pml4es[i].base & P))
		{
			return &pml4es[i];
		}
	}
	return 0;
}

static returnValue FindFreePTFromPML4(PML4E *pml4es, void *address)
{
	int index = 0;
research:
	PML4E *exeistent = FindExeistentPML4(pml4es, index);
	if (exeistent)
	{
		index = ((UINT64)exeistent & 0xfff) / sizeof(PML4E);
		if (index >= 0x1000 / sizeof(PML4E))
			return (returnValue){0};
		returnValue value = FindFreePTFromPDPT((PDPTE *)(((UINT64)pml4es << 9) | ((UINT64)index << 12)), (void *)((UINT64)address | ((UINT64)index << (12 + 9 + 9 + 9))));
		if (value.pte)
		{
			return value;
		}
		else
		{
			goto research;
		}
	}
	else
	{
		PML4E *FreePML4 = FindFreePML4(pml4es);
		index = ((UINT64)FreePML4 & 0xfff) / sizeof(PML4E);
		MMAP *FreeMmap = FindAFreeMMAP();
		FreePML4->base = FreeMmap->PhysicalAddress | P | R_W;
		FreePML4->high = (UINT32)(FreeMmap->PhysicalAddress >> 32);
		returnValue value;
		switch (HowManyCanPut())
		{
		case 0:
			UINT64 PhysicalAddress = FreeMmap->PhysicalAddress;
			MMAP *Previous = FindThePreviousMMAPOf(FreeMmap);
			FreeMmap->PhysicalAddress += 0x1000;
			value = FindFreePTFromPDPT((PDPTE *)(((UINT64)pml4es << 9) | ((UINT64)index << 12)), (void *)((UINT64)address | ((UINT64)index << (12 + 9 + 9 + 9))));
			*configure.nextWrite = (MMAP){
				.type = MAP_TYPE_OSDATA,
				.flags = MAP_FLAG_READ | MAP_FLAG_WRITE,
				.NoOfPage = 1,
				.PhysicalAddress = PhysicalAddress,
				.VirtualAddress = (void *)(((UINT64)address << 9) | (UINT64)index << 12),
				.next = Previous->next ? Previous->next : &Previous[1]};
			Previous->next = configure.nextWrite;
			configure.nextWrite += 0x1000;
			break;
		default:
			*configure.nextWrite = (MMAP){
				.type = MAP_TYPE_OSDATA,
				.flags = MAP_FLAG_READ | MAP_FLAG_WRITE,
				.NoOfPage = 1,
				.PhysicalAddress = PhysicalAddress,
				.VirtualAddress = (void *)(((UINT64)address << 9) | (UINT64)index << 12),
				.next = Previous->next ? Previous->next : &Previous[1]};
			FindThePreviousMMAPOf(FreeMmap)->next = configure.nextWrite;
			configure.nextWrite += 0x1000;
			FreeMmap->PhysicalAddress += 0x1000;
			value = FindFreePTFromPDPT((PDPTE *)(((UINT64)pml4es << 9) | ((UINT64)index << 12)), (void *)((UINT64)address | ((UINT64)index << (12 + 9 + 9 + 9))));
			break;
		}
		return value;
	}
}

static returnValue FindFreePTFromRoot()
{
	#if X86
	return FindFreePTFromPD((PDE *)0xfffff000, 0x0);
	#elif X86_64
	return FindFreePTFromPML4((PML4E *)0xfffffffffffff000, 0x0);
	#endif
}

void *MapAPageInFreeVirtualAddress(UINT64 PhysicalAddress, int type, UINT64 flags)
{
	if (type == PageSize4KB)
	{
		returnValue value = FindFreePTFromRoot();
		PTE *pte = value.pte;
		pte->base = (UINT32)PhysicalAddress | (UINT32)flags;
		pte->high = (UINT32)(PhysicalAddress >> 32) | (UINT32)(flags >> 32);
		io_out_cr3_64(io_in_cr3_64());
		return value.address;
	}
}