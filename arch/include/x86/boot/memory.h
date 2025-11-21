#ifndef BOOT_MEMORY
#define BOOT_MEMORY

#include "acpi/ARDS.h"
#include "btype.h"
#include "x86/memory.h"
#include "elf_kernel/elf.h"

void setVirtualMemory();
void addVirtualMemory(UINT64 PhysicalAddress, UINT64 VirtualAddress, UINT32 PageCount, UINT32 flags_low, UINT32 flags_high, UINT32 type);
UINT64 add_map_at(MMAP *mmap, UINT64 PhysicalAddress, UINT8 type, UINT8 flags, UINT64 NoOfPage, MMAP *next);
void ModifyMMAP(MMAP *mmap, ARD *ards, UINT32 count, Elf64_Ehdr *ehdr);

#endif
