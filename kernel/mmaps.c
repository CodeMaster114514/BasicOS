#include "kernel.h"
#include "btype.h"
#include "memory.h"
#include "mmaps.h"

extern MemoryConfigure configure;

void addMMAPAfter(MMAP *mmap, MMAP *target)
{
    if (mmap->isLast)
    {
        mmap->isLast = false;
        target->isLast = true;
        mmap->next = target;
    }
    else
    {
	    target->next = mmap->next ? mmap->next : &mmap[1];
    	mmap->next = target;
    }
}

void addMMAPBefore(MMAP *mmap, MMAP *target)
{
	target->next = mmap;
	findThePreviousMMAPOf(mmap)->next = target;
}

MMAP *findAFreeMMAP()
{
	MMAP *mmap = configure.mmap;
	while (1)
	{
		if (mmap->type == MAP_TYPE_FREE_MEMORY)
		{
			return mmap;
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
	return 0;
}

MMAP *findThePreviousMMAPOf(MMAP *mmap)
{
	MMAP *previous = configure.mmap;
	while (1)
	{
		if ((previous->next ? previous->next : &previous[1]) == mmap)
		{
			return previous;
		}
		if (previous->isLast)
		{
			break;
		}
		else
		{
			previous = previous->next ? previous->next : &previous[1];
		}
	}
	return 0;
}

MMAP *findA_NULL_MMAP()
{
	MMAP *null_MMAP = configure.mmap;
	while (1)
	{
		if (null_MMAP->type == MAP_TYPE_NULL)
		{
			return null_MMAP;
		}
		if (null_MMAP->isLast)
		{
			break;
		}
		else
		{
			null_MMAP = null_MMAP->next ? null_MMAP->next : &null_MMAP[1];
		}
	}
	if (configure.isFull)
		return NULL;
	else
		return configure.nextWrite;
}

MMAP *findNextFreeMMAP(MMAP *mmap)
{
	MMAP *next = mmap->next;
	while (1)
	{
		if (next->type == MAP_TYPE_FREE_MEMORY)
		{
			return next;
		}
		if (next->isLast)
		{
			break;
		}
		else
		{
			next = next->next ? next->next : &next[1];
		}
	}
	return 0;
}

MMAP *findAFreeMMAPWithHave2MBPage()
{
	MMAP *mmap = findAFreeMMAP();
	while (mmap)
	{
		if (mmap->NoOfPage >= 512)
		{
			UINT64 StartAddress = mmap->PhysicalAddress;
			UINT64 StartAddress2MB = StartAddress & 0xffffffffffe00000 + StartAddress & 0x1fffff ? 0x1fffff : 0;
			UINT64 skipPage = (StartAddress2MB - StartAddress) >> 12;
			UINT64 realPage = mmap->NoOfPage - skipPage;
			if (realPage >= 512)
				return mmap;
		}
		mmap = findNextFreeMMAP(mmap);
	}
	return 0;
}

void checkNextWrite()
{
	if ((UINT64)configure.nextWrite - (UINT64)configure.nextWriteStart >= 4096)
	{
		configure.isFull = true;
	}
}

int HowManyCanPut()
{
	return (0x1000 - ((UINT64)configure.nextWrite & 0xfff)) / sizeof(MMAP);
}
