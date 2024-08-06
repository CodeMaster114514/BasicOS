#include "btype.h"
#include "memory.h"
#include "config.h"

MemoryConfigure configure = {0};

static MMAP MmapCache = {0};

void InitMemory(MMAP *mmap, UINT64 count, int LinearAddrSize, int PhysicalAddrSize)
{
	InitPaging(LinearAddrSize, PhysicalAddrSize);
	configure.mmap = mmap;
	configure.MmapCount = count;
	configure.nextWrite = &mmap[count];
	while (1)
	{
		if (mmap->PhysicalAddress < 0x20000 && mmap->type == MAP_TYPE_FREE_MEMORY)
		{
			mmap->type = MAP_TYPE_OSDATA;
			mmap->flags = MAP_FLAG_EXECTION | MAP_FLAG_READ | MAP_FLAG_WRITE;
			break;
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
}

void *allocA4KBPage(UINT8 type, UINT8 flags)
{
	MMAP *mmap = FindAFreeMMAP();
	MmapCache = (MMAP){
		.type = MAP_TYPE_OSDATA,
		.flags = flags,
		.NoOfPage = 1,
		.PhysicalAddress = mmap->PhysicalAddress,
	};
	UINT64 PageFlags = 0;
	switch (type)
	{
	case MAP_TYPE_USER_CODE:
		PageFlags |= U_S;
	case MAP_TYPE_OSCODE:
		PageFlags |= X86 ? PATMode ? NX : 0 : XD;
		break;
	}
	PageFlags = PageFlags |
				(flags & MAP_FLAG_WRITE ? R_W : 0) |
				P;
	UINT64 PhysicalAddress = mmap->PhysicalAddress;
	mmap->PhysicalAddress += 0x1000;
	mmap->NoOfPage -= 1;
	void *target = MapAPageInFreeVirtualAddress(PhysicalAddress, PageSize4KB, PageFlags);
	*configure.nextWrite = MmapCache;
	configure.nextWrite->next = mmap;
	FindThePreviousMMAPOf(mmap)->next = configure.nextWrite;
	configure.nextWrite += sizeof(MMAP);
	return target;
}