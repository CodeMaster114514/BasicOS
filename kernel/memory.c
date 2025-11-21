#include "kernel.h"
#include "btype.h"
#include "memory.h"
#include "config.h"
#include "shell.h"
#include "mmaps.h"
#include "x86/io/io64.h"

MemoryConfigure configure = {0};

static MMAP MmapCache = {0};

void InitMemory(MMAP *mmap, UINT64 count, int LinearAddrSize, int PhysicalAddrSize)
{
	// InitPaging(LinearAddrSize, PhysicalAddrSize);
	configure.mmap = mmap;
	configure.MmapCount = count;
	configure.nextWrite = &mmap[count];
	configure.nextWriteStart = (void *)((UINT64)configure.nextWrite & 0xfffffffffffff000);
	configure.isFull = false;
	configure.debug = false;
	while (1)
	{
		if (mmap->PhysicalAddress < 0x200000 && mmap->type == MAP_TYPE_FREE_MEMORY)
		{
			if (mmap->PhysicalAddress + mmap->NoOfPage * 0x1000 > 0x200000)
			{
				UINT32 pageCount = (0x200000 - mmap->PhysicalAddress) / 0x1000;
				*configure.nextWrite = (MMAP){
					.type = MAP_TYPE_FREE_MEMORY,
					.flags = 0,
					.NoOfPage = pageCount,
					.PhysicalAddress = mmap->PhysicalAddress + pageCount * 0x1000,
					.isLast = false,
				};
				addMMAPAfter(mmap, configure.nextWrite);
				configure.nextWrite = &configure.nextWrite[1];
			}
			else
			{
				mmap->type = MAP_TYPE_OSDATA;
				mmap->flags = MAP_FLAG_EXECTION | MAP_FLAG_READ | MAP_FLAG_WRITE;
			}
		}
		if (mmap->isLast)
		{
			break;
		}
		else
		{
			mmap = mmap->next ? mmap->next : &mmap[1];
		}
	}
#if X86
	resetGDT();
	// resetSP();
#endif
}

void cleanMemory(void *target_address, UINT64 size)
{
	UINT64 *target = target_address;
	for (UINT64 i = 0; i < size / sizeof(UINT64); ++i)
	{
		target[i] = 0;
	}
}

int check1231 = 0;
void breakpoint2()
{
}

bool FixVirtualMemoryByAPage(PageDescriptor ErrorPage, MMAP *freeMemroy)
{
	PDE *pde;
	PDPTE *pdpt;
	PML4E *pml4e;
	switch (ErrorPage.status)
	{
	case GetPageNeedANewPTEs:
		pde = ErrorPage.target;
		pde->base = (UINT32)freeMemroy->PhysicalAddress | P | R_W;
		pde->high = (UINT32)(freeMemroy->PhysicalAddress >> 32);
		break;

	case GetPageNeedANewPDEs:
		pdpt = ErrorPage.target;
		pdpt->base = (UINT32)freeMemroy->PhysicalAddress | P | R_W;
		pdpt->high = (UINT32)(freeMemroy->PhysicalAddress >> 32);
		break;

	case GetPageNeedANewPDPTEs:
		pml4e = ErrorPage.target;
		pml4e->base = (UINT32)freeMemroy->PhysicalAddress | P | R_W;
		pml4e->high = (UINT32)(freeMemroy->PhysicalAddress >> 32);
		break;

	default:
		return false;
	}
	io_out_cr3_64(io_in_cr3_64());
	return true;
}

bool FixVirtualMemory(PageDescriptor ErrorPage, UINT8 TestType, bool position, MMAP *mmapCache)
{
	MMAP *freeMMAP = findAFreeMMAP();
	if (freeMMAP == 0)
	{
		puts("There is no free memory space.\n");
		return false;
	}
	UINT64 PageCount, PhysicalAddress = freeMMAP->PhysicalAddress;
	++PageCount;
	freeMMAP->PhysicalAddress += 0x1000;
	FixVirtualMemoryByAPage(ErrorPage, freeMMAP);
	while (1)
	{
		ErrorPage = findAFreePage(TestType, position);
		if (ErrorPage.status == GetPageSuccess)
		{
			break;
		}
		FixVirtualMemoryByAPage(ErrorPage, freeMMAP);
		++PageCount;
		freeMMAP->PhysicalAddress += 0x1000;
		if (PageCount == freeMMAP->NoOfPage)
			;
		{
			freeMMAP->PhysicalAddress = PhysicalAddress;
			freeMMAP->type = MAP_TYPE_OSDATA;
			freeMMAP = findAFreeMMAP();
			if (freeMMAP == 0)
			{
				puts("We have to free memory to fix Virtual Memory\n");
				return false;
			}
			PageCount = 0;
			PhysicalAddress = freeMMAP->PhysicalAddress;
		}
	}
	if (PageCount != 0)
	{
		mmapCache->type = MAP_TYPE_OSDATA;
		mmapCache->PhysicalAddress = PhysicalAddress;
		mmapCache->NoOfPage = PageCount;
		mmapCache->flags = MAP_FLAG_WRITE | MAP_FLAG_READ;
	}
	return true;
}

bool set = false;

bool ExpandMMAP()
{
	if (set)
	{
		puts("data: %d\n", *(UINT32 *)0x40800000);
	}
	MMAP *freeMMAP = findAFreeMMAP(), *nullMMAP;
	if (freeMMAP == NULL)
	{
		puts("There is no free memory space.\n");
		return false;
	}

	UINT32 PageCount = freeMMAP->NoOfPage;
	MMAP cache = {.type = MAP_TYPE_NULL};
	PageDescriptor page = findAFreePage(PageSize4KB, true);
	if (page.status != GetPageSuccess)
	{
		if (!FixVirtualMemory(page, PageSize4KB, true, &cache))
		{
			return 0;
		}
		page = findAFreePage(PageSize4KB, true);
		freeMMAP = findAFreeMMAP();
		if (freeMMAP == NULL)
		{
			puts("There is no free memory space.\n");
			return false;
		}
	}
	puts("%p\n", page.VirtualAddress);
	PTE *pte = page.target;
	--PageCount;
	pte->base = (UINT32)freeMMAP->PhysicalAddress | P | R_W;
	pte->high = (UINT32)((freeMMAP->PhysicalAddress | XD) >> 32);
	io_out_cr3_64(io_in_cr3_64()); // 刷写TLB
	freeMMAP->PhysicalAddress = freeMMAP->PhysicalAddress + 0x1000 * (freeMMAP->NoOfPage - PageCount);
	freeMMAP->NoOfPage = PageCount;
	nullMMAP = page.VirtualAddress;
	if (cache.type != MAP_TYPE_NULL)
	{
		*nullMMAP = cache;
		addMMAPBefore(freeMMAP, nullMMAP);
	}
	configure.isFull = false;
	configure.nextWriteStart = nullMMAP;
	puts("%p\n", configure.nextWriteStart);
	configure.nextWrite = &nullMMAP[1];
	puts("Expand MMAP Success\n");
	if (set)
	{
		puts("data: %d\n", *(UINT32 *)0x40800000);
	}
	return true;
}

void *allocA4KBPage(UINT8 type, UINT8 flags)
{
	bool position = false;
	if (type == MAP_TYPE_OSDATA || type == MAP_TYPE_OSCODE)
	{
		position = true;
	}

	MMAP *freeMMAP = findAFreeMMAP();
	if (freeMMAP == 0)
	{
		puts("There is no free memory space.\n");
		return 0;
	}

	MMAP *nullMMAP = findA_NULL_MMAP();

	if (!nullMMAP)
	{
		if (!ExpandMMAP())
		{
			return 0;
		}
		nullMMAP = findA_NULL_MMAP();
		freeMMAP = findAFreeMMAP();
		if (freeMMAP == 0)
		{
			puts("There is no free memory space.\n");
			return 0;
		}
	}

	PageDescriptor page = findAFreePage(PageSize4KB, position);
	if (page.status != GetPageSuccess)
	{
		if (FixVirtualMemory(page, PageSize4KB, position, nullMMAP) == false)
		{
			puts("There is no free memory space.\n");
			return 0;
		}
		nullMMAP = findA_NULL_MMAP();
		if (!nullMMAP)
		{
			if (!ExpandMMAP())
			{
				return 0;
			}
			nullMMAP = findA_NULL_MMAP();
		}
		freeMMAP = findAFreeMMAP();
		if (freeMMAP == 0)
		{
			puts("There is no free memory space.\n");
			return 0;
		}
		page = findAFreePage(PageSize4KB, position);
	}

	UINT32 flags_low = 0, flags_high = 0;

	if (flags | MAP_FLAG_WRITE)
	{
		flags_low = R_W;
	}

	if (type == MAP_TYPE_OSCODE)
	{
	}
	else if (type == MAP_TYPE_OSDATA)
	{
		flags_high = XD >> 32;
	}
	else if (type == MAP_TYPE_USER_CODE)
	{
		flags_low = U_S;
	}
	else if (type == MAP_TYPE_USER_DATA)
	{
		flags_low = U_S;
		flags_high = XD >> 32;
	}
	PTE *pte = page.target;
	pte->base = (UINT32)freeMMAP->PhysicalAddress | P | flags_low;
	pte->high = (UINT32)(freeMMAP->PhysicalAddress >> 32) | flags_high;

	if (freeMMAP->NoOfPage - 1 == 0)
	{
		freeMMAP->type = type;
		freeMMAP->flags = flags;
	}
	else
	{
		if (check1231 >= 5000)
		{
			puts("%d\n", check1231);
			check1231 = 0;
		}
		puts("%d\n", check1231);
		if (check1231 == 4728)
		{
			breakpoint2();
		}
		++check1231;
		puts("%p\n", nullMMAP);
		nullMMAP->type = type;
		nullMMAP->flags = flags;
		nullMMAP->PhysicalAddress = freeMMAP->PhysicalAddress;
		nullMMAP->NoOfPage = 1;
		nullMMAP->next = 0;
		nullMMAP->isLast = false;
		puts("%p\n", nullMMAP);
		freeMMAP->PhysicalAddress += 0x1000;
		--freeMMAP->NoOfPage;
		addMMAPBefore(freeMMAP, nullMMAP);
		if (nullMMAP == configure.nextWrite)
		{
			configure.nextWrite = &configure.nextWrite[1];
			checkNextWrite();
		}
	}
	return page.VirtualAddress;
}

UINT64 alignAddressTo2M(UINT64 address)
{
	if (address & 0x1fffff)
	{
		address = (address & 0xffffffffffe00000) + (address & 0x1fffff ? 0x200000 : 0); // align to 2MB
	}
	return address;
}

int times = 0;

void *allocA2MBPage(UINT8 type, UINT8 flags)
{
	times++;
	bool position = false;
	if (type == MAP_TYPE_OSDATA || type == MAP_TYPE_OSCODE)
	{
		position = true;
	}

	MMAP *freeMMAP = findAFreeMMAPWithHave2MBPage();
	if (freeMMAP == 0)
	{
		puts("There is no free memory space.\n");
		return 0;
	}

	MMAP *nullMMAP = findA_NULL_MMAP();

	if (!nullMMAP)
	{
		ExpandMMAP();
		nullMMAP = findA_NULL_MMAP();
		freeMMAP = findAFreeMMAPWithHave2MBPage();
		if (freeMMAP == 0)
		{
			puts("There is no free memory space.\n");
			return 0;
		}
	}
	//if (set)
	//{
	//	puts("data: %d\n", *(UINT32 *)0x40800000);
	//}

	PageDescriptor page = findAFreePage(PageSize2MB, position);
	if (page.status != GetPageSuccess)
	{
		if (FixVirtualMemory(page, PageSize2MB, position, nullMMAP) == false)
		{
			puts("There is no free memory space.\n");
			return 0;
		}
		nullMMAP = findA_NULL_MMAP();
		if (!nullMMAP)
		{
			if (!ExpandMMAP())
			{
				return 0;
			}
			nullMMAP = findA_NULL_MMAP();
		}
		freeMMAP = findAFreeMMAPWithHave2MBPage();
		if (freeMMAP == 0)
		{
			puts("There is no free memory space.\n");
			return 0;
		}
		page = findAFreePage(PageSize2MB, position);
	}

	UINT32 flags_low = 0, flags_high = 0;

	if (flags | MAP_FLAG_WRITE)
	{
		flags_low = R_W;
	}

	if (type == MAP_TYPE_OSCODE)
	{
	}
	else if (type == MAP_TYPE_OSDATA)
	{
		flags_high = XD >> 32;
	}
	else if (type == MAP_TYPE_USER_CODE)
	{
		flags_low = U_S;
	}
	else if (type == MAP_TYPE_USER_DATA)
	{
		flags_low = U_S;
		flags_high = XD >> 32;
	}
	PDE *pde = page.target;
	UINT64 alignedAddress = alignAddressTo2M(freeMMAP->PhysicalAddress);
	pde->base = (UINT32)alignedAddress | P | PS2MB | flags_low;
	pde->high = (UINT32)(alignedAddress >> 32) | flags_high;

	puts("Alloc 2MB Page at %p, pde: %p\n", page.VirtualAddress, *(UINT64 *)&pde->base);
	// puts("From freeMMAP at %p, PhysicalAddress: %p\n", freeMMAP, freeMMAP->PhysicalAddress);

		UINT64 totalPage = freeMMAP->NoOfPage;
		if (alignAddressTo2M(freeMMAP->PhysicalAddress) != freeMMAP->PhysicalAddress)
		{
			UINT64 SkipPage = (alignedAddress - freeMMAP->PhysicalAddress) >> 12;
			freeMMAP->NoOfPage = SkipPage;
			alignedAddress = alignAddressTo2M(freeMMAP->PhysicalAddress);
			totalPage -= SkipPage;
			*nullMMAP = (MMAP){
				.type = type,
				.flags = flags,
				.PhysicalAddress = alignedAddress,
				.NoOfPage = 512,
				.next = 0,
				.isLast = false};
			addMMAPAfter(freeMMAP, nullMMAP);
			if (nullMMAP == configure.nextWrite)
			{
				configure.nextWrite = &configure.nextWrite[1];
				checkNextWrite();
			}
			if (totalPage - 512 != 0)
			{
				MMAP *last = nullMMAP;
				nullMMAP = findA_NULL_MMAP();
				if (!nullMMAP)
				{
					if (!ExpandMMAP())
					{
						return 0;
					}
					nullMMAP = findA_NULL_MMAP();
				}
				*nullMMAP = (MMAP){
					.type = MAP_TYPE_FREE_MEMORY,
					.flags = 0,
					.PhysicalAddress = alignedAddress + (512 << 12),
					.NoOfPage = totalPage - 512,
					.isLast = false};
				addMMAPAfter(last, nullMMAP);
				if (nullMMAP == configure.nextWrite)
				{
					configure.nextWrite = &configure.nextWrite[1];
					checkNextWrite();
				}
			}
		}
		else
		{
			freeMMAP->NoOfPage = 512;
			freeMMAP->type = type;
			freeMMAP->flags = flags;
			if (totalPage - 512 != 0)
			{
				nullMMAP->type = MAP_TYPE_FREE_MEMORY;
				nullMMAP->flags = 0;
				nullMMAP->PhysicalAddress = alignedAddress + (512 << 12);
				nullMMAP->NoOfPage = totalPage - 512;
				nullMMAP->isLast = false;
				nullMMAP->next = 0;
				addMMAPAfter(freeMMAP, nullMMAP);
				if (nullMMAP == configure.nextWrite)
				{
					configure.nextWrite = &configure.nextWrite[1];
					checkNextWrite();
				}
			}
		}
	return page.VirtualAddress;
}
