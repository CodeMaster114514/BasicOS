#include "btype.h"
#include "memory.h"

extern MemoryConfigure configure;

void addMMAPAfter(MMAP *mmap, MMAP *target)
{
	target->next = mmap->next;
	mmap->next = target;
}

void addMMAPBefore(MMAP *mmap, MMAP *target)
{
	target->next = mmap;
	FindThePreviousMMAPOf(mmap)->next = target;
}

MMAP *FindAFreeMMAP()
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

MMAP *FindThePreviousMMAPOf(MMAP *mmap)
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

int HowManyCanPut()
{
    return (0x1000 - ((UINT64)configure.nextWrite & 0xfff)) / sizeof(MMAP);
}
