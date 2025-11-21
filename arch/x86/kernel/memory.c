#include "kernel.h"
#include "shell.h"
#include "video.h"
#include "memory.h"
#include "x86/memory.h"
#include "x86/io/io64.h"

PageDescriptor findAFreePageFromPTEs(PTE *pte, UINT32 type)
{
	// 在内存中查找一个空闲的页描述符
	PageDescriptor cache;
	for (int i = 0; i < 512; i++)
	{
		if (!(pte[i].base & P))
		{
			cache.target = (void *)&pte[i];
			cache.VirtualAddress = (void *)((UINT64)pte << 9 | i << 12);
			cache.status = GetPageSuccess;

			if (!((UINT64)cache.VirtualAddress & 0x0000800000000000))
			{
				cache.VirtualAddress = (void *)((UINT64)cache.VirtualAddress & 0x0000ffffffffffff);
			}
			return cache;
		}
	}
	cache.status = GetPageRunOutOfPT;
	return cache;
}

PageDescriptor findAFreePageFromPDEs(PDE *pde, UINT32 type)
{
	// 在内存中查找一个空闲的页描述符
	PageDescriptor cache;
	for (int i = 0; i < 512; i++)
	{
		if ((pde[i].base & P) && (!(pde[i].base & PS2MB)) && type == PageSize4KB)
		{
			puts("pde[%d] address: %p data: %p\n", i, (void *)&pde[i], *(UINT64 *)&pde[i].base);
			cache = findAFreePageFromPTEs((UINT64)pde << 9 | i << 12, type);
			if (cache.status == GetPageSuccess)
				return cache;
		}
		else if ((!(pde[i].base & P)) && type == PageSize2MB)
		{
			cache.target = (void *)&pde[i];
			cache.VirtualAddress = (void *)(((UINT64)pde << 9 | i << 12) << 9);
			cache.status = GetPageSuccess;
			puts("index: %d, vaddr: %p\n", i, cache.VirtualAddress);

			if (!((UINT64)cache.VirtualAddress & 0x0000800000000000))
			{
				cache.VirtualAddress = (void *)((UINT64)cache.VirtualAddress & 0x0000ffffffffffff);
			}
			return cache;
		}
		if (i > 226 && i <231)
		{
			puts("pde[%d] address: %p data: %p\n", i, (void *)&pde[i], *(UINT64 *)&pde[i].base);
		}
	}
	for (int i = 0; i < 512; i++)
	{
		if (!(pde[i].base & P))
		{
			cache.target = &pde[i];
			cache.status = GetPageNeedANewPTEs;
			return cache;
		}
	}
	cache.status = GetPageRunOutOfPD;
	return cache;
}

PageDescriptor findAFreePageFromPDPTEs(PDPTE *pdpte, UINT32 type)
{
	// 在内存中查找一个空闲的页描述符
	PageDescriptor cache;
	bool hasFreePDPT;
	for (int i = 0; i < 512; i++)
	{
		if ((pdpte[i].base & P) && (!(pdpte[i].base & PS1GB)) && (type == PageSize4KB || type == PageSize2MB))
		{
			cache = findAFreePageFromPDEs((UINT64)pdpte << 9 | i << 12, type);
			if (cache.status == GetPageSuccess || cache.status == GetPageNeedANewPTEs)
				return cache;
		}
		else if ((!(pdpte[i].base & P)) && type == PageSize1GB)
		{
			cache.target = (void *)&pdpte[i];
			cache.VirtualAddress = (void *)(((UINT64)pdpte << 9 | i << 12) << (9 * 2));
			cache.status = GetPageSuccess;

			if (!((UINT64)cache.VirtualAddress & 0x0000800000000000))
			{
				cache.VirtualAddress = (void *)((UINT64)cache.VirtualAddress & 0x0000ffffffffffff);
			}
			return cache;
		}
	}
	for (int i = 0; i < 512; i++)
	{
		if (!(pdpte[i].base & P))
		{
			cache.target = (void *)&pdpte[i];
			cache.status = GetPageNeedANewPDEs;
			return cache;
		}
	}
	cache.status = GetPageRunOutOfPDPT;
	return cache;
}

PageDescriptor findAFreePageFromPML4s(PML4E *pml4e, UINT32 type, bool position)
{
	// 在内存中查找一个空闲的页描述符
	PageDescriptor cache;
	bool hasFreePML4E;
	UINT32 start = position ? 256 : 0;
	UINT32 end = position ? 511 : 255;
	for (int i = start; i < end; i++) // 最后一个不找是因为它指向了PML4自己
	{
		if (pml4e[i].base & P)
		{
			cache = findAFreePageFromPDPTEs((UINT64)pml4e << 9 | i << 12, type);
			if (cache.status == GetPageSuccess || cache.status == GetPageNeedANewPDEs || cache.status == GetPageNeedANewPTEs)
				return cache;
		}
	}
	for (int i = start; i < end; i++)
	{
		if (!(pml4e[i].base & P))
		{
			cache.target = (void *)&pml4e[i];
			cache.status = GetPageNeedANewPDPTEs;
		}
	}
	cache.status = GetPageRunOutOfPML4;
	return cache;
}

PageDescriptor findAFreePage(UINT32 type, bool position)
{
	// 在内存中查找一个空闲的页描述符
	PML4E *pml4e = (PML4E *)0xfffffffffffff000; // PML4的线性地址，因为在PML4中的最顶端的PDPTE指向了它自己
	return findAFreePageFromPML4s(pml4e, type, position);
}

void *mountAPageInFreeVirtulaMemory(void *physicalAddress, UINT32 type, UINT64 flags, bool position)
{
	PTE *pte;
	PDE *pde;
	PDPTE *pdpte;
	PageDescriptor cache = findAFreePage(type, position);
	if (!cache.target)
	{
		// 如果没有找到合适的空闲页，返回0
		// 后期可以通过调用中断来提醒用户或自动清理虚拟内存
		return 0;
	}

	switch (type)
	{
	case PageSize4KB:
		pte = (PTE *)cache.target;
		*(UINT64 *)pte = (UINT64)physicalAddress | P | flags;
		break;

	case PageSize2MB:
		pde = (PDE *)cache.target;
		*(UINT64 *)pde = (UINT64)physicalAddress | P | flags | PS2MB;
		break;

	case PageSize1GB:
		pdpte = (PDPTE *)cache.target;
		*(UINT64 *)pdpte = (UINT64)physicalAddress | P | flags | PS1GB;
		break;

	default:
		break;
	}

	return cache.VirtualAddress;
}

void resetGDT()
{
	GDTR gdtr;
	getGDTR(&gdtr);
	gdtr.base |= 0xffff800000000000;
	setGDTR(&gdtr);
}

void resetSP()
{
	void *new_sp = allocA2MBPage(MAP_TYPE_OSDATA, MAP_FLAG_READ | MAP_FLAG_WRITE);
	void *sp;
	asm("mov %0, rsp\n\t"
		: "=m"(sp)
		:
		:);
	//copyDataTo(0x0, new_sp, 0x7c00);
}
