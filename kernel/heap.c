#include "kernel.h"
#include "heap.h"
#include "memory.h"

Heaps *heaps;

void initHeap()
{
	heaps = allocA4KBPage(MAP_TYPE_OSDATA, MAP_FLAG_READ | MAP_FLAG_WRITE);
	heaps->next = NULL;
}