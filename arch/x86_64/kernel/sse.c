#include "x86-and-x86_64/sse.h"
#include "shell.h"
#include "video.h"

void EnableSSE(UINT64 cpuExInfo)
{
	if (cpuExInfo & (SSE | SSE2))
	{
		asm(
			"mov rax, cr0\n\t"
			"and ax, 0xfffb\n\t"
			"or ax, 2\n\t"
			"mov cr0, rax\n\t"
			"mov rax, cr4\n\t"
			"or eax, 0x40600\n\t"
			"mov cr4, rax\n\t"
			"xor ecx, ecx\n\t"
			"xgetbv\n\t"
			"or eax, 3\n\t"
			"xsetbv\n\t"
			:
			:
			:"eax", "ecx", "edx"
		);
	}
	else
	{
		for (;;)
		{
			putc('x', 0xffff);
			asm(
				"hlt"
			);
		}
	}
}
