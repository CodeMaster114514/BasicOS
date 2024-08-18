#include "config.h"
#include "btype.h"
#include "memory.h"
#include "video.h"
#include "shell.h"
#include "interrupt.h"
#if X86_64

#include "x86-and-x86_64/sse.h"

#endif

int a = 9;
const char *b = "abc";

int kernel(Table *table)
{
	InitVideo(&table->gconfigre);
#if X86_64
	EnableSSE();
#endif
	InitMemory(table->mmap, table->map_count, table->LinearAddrSize, table->PhysicalAddrSize);
	InitInterrupt();
	puts("We are running in the ia32e mode\n");
	puts("The number is %d\n", 512);
	for(;;)
	{
		asm("hlt\n\t");
	}
}