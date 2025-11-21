#include "kernel.h"
#include "x86/cpuid.h"

int showCPUInfo()
{
    int eax, ebx, ecx, edx, total;
    asm volatile(
        "cpuid\n\t"
        : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
        : "a"(0));

    total = puts("CPU Info:\n");
    total += puts("Vendor ID: %c%c%c%c%c%c%c%c%c%c%c%c\n", ebx & 0xff, (ebx >> 8) & 0xff, (ebx >> 16) & 0xff, (ebx >> 24) & 0xff,
                  edx & 0xff, (edx >> 8) & 0xff, (edx >> 16) & 0xff, (edx >> 24) & 0xff, ecx & 0xff, (ecx >> 8) & 0xff, (ecx >> 16) & 0xff, (ecx >> 24) & 0xff);
    total += puts("Family: %d\n", (eax >> 8) & 0xf);
    total += puts("Model: %d\n", (eax >> 4) & 0xf);
    total += puts("Stepping: %d\n", eax & 0xf);

    return total;
}

int showCPUBrand()
{
    int eax, ebx, ecx, edx, total = 0;
    total += puts("CPU Brand: ");
    for (int FunctionID = 0x80000002; FunctionID <= 0x80000004; FunctionID++)
    {
        asm(
            "cpuid\n\t"
            : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
            : "a"(FunctionID));
        total += puts("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
                      eax & 0xff, (eax >> 8) & 0xff, (eax >> 16) & 0xff, (eax >> 24) & 0xff,
                      ebx & 0xff, (ebx >> 8) & 0xff, (ebx >> 16) & 0xff, (ebx >> 24) & 0xff,
                      ecx & 0xff, (ecx >> 8) & 0xff, (ecx >> 16) & 0xff, (ecx >> 24) & 0xff,
                      edx & 0xff, (edx >> 8) & 0xff, (edx >> 16) & 0xff, (edx >> 24) & 0xff);
    }
    putc('\n', 0xffff); // 换行
    return total + 1; // +1 for the newline character
}