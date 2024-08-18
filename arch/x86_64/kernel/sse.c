#include "x86-and-x86_64/sse.h"
#include "shell.h"
#include "video.h"

UINT32 DataOfECX, DataOfEDX;

void EnableSSE() // 启用拓展
{
	UINT32 result = 0;
	asm( // 启用XCR0的读写
		"mov rax, cr4\n\t"
		"or eax, 0x40200\n\t"
		"mov cr4, rax\n\t"
		:
		:
		:"eax"
	);
	asm(
		"mov eax, 0\n\t"
		"cpuid\n\t"
		"mov %0, ecx\n\t"
		"mov %1, edx\n\t"
		:"=m"(DataOfECX), "=m"(DataOfEDX)
		:
		:"eax", "ecx", "edx"
	);
	if (DataOfEDX & CPUID_FEAT_EDX_SSE)
	{
		result |= 0b10;
	}
	if (DataOfECX & CPUID_FEAT_ECX_AVX)
	{
		result |= 0b100;
	}
	asm(
		"xor ecx, ecx\n\t"
		"xgetbv\n\t"
		"or eax, %0\n\t"
		"xsetbv\n\t"
		:
		:"m"(result)
		:"eax", "ecx"
	);
}
