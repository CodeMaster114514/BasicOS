#include "btype.h"
#include <linux/elf.h>
#include "acpi/ARDS.h"
#include "x86-and-x86_64/cpuid.h"

typedef void (*KERNEL_ENTER)(Table *Table);

UINT64 add_map_at(MMAP *mmap, void *PhysicalAddress, void *VirtualAddress, UINT8 type, UINT8 flags, UINT64 NoOfPage, MMAP *next)
{
	UINT64 used_count;
	bool isLast = false;
	while (1) {
		isLast = mmap->isLast;
		if (mmap->PhysicalAddress <= PhysicalAddress && mmap->PhysicalAddress + (mmap->NoOfPage << 12) > PhysicalAddress)
		{
			if (PhysicalAddress == mmap->PhysicalAddress)
			{
				*next = *mmap;
				next->NoOfPage -= NoOfPage;
				next->PhysicalAddress += NoOfPage << 12;
				next->VirtualAddress += NoOfPage << 12;
				next->next = mmap->next ? mmap->next : &mmap[1];
				mmap->type = type;
				mmap->flags = flags;
				mmap->VirtualAddress = VirtualAddress;
				mmap->NoOfPage = NoOfPage;
				mmap->isLast = false;
				mmap->next = next;
				used_count = 1;
				break;
			}
			else
			{
				UINT64 NoOfPageCache = mmap->NoOfPage;
				mmap->NoOfPage = (UINT64)(PhysicalAddress - mmap->PhysicalAddress) >> 12;
				bool NeedThirdMap = mmap->NoOfPage + NoOfPage < NoOfPageCache;
				*next = (MMAP)
				{
					.type = type,
					.flags = flags,
					.NoOfPage = NoOfPage,
					.isLast = false,
					.PhysicalAddress = PhysicalAddress,
					.VirtualAddress = VirtualAddress,
					.next = NeedThirdMap ? 0 : (mmap->next ? mmap->next : &mmap[1])
				};
				used_count = 1;
				if (NeedThirdMap)
				{
					next[1] = (MMAP)
					{
						.type = type,
						.flags = flags,
						.NoOfPage = NoOfPageCache - mmap->NoOfPage - NoOfPage,
						.PhysicalAddress = next->PhysicalAddress + (next->NoOfPage << 12),
						.VirtualAddress = mmap->VirtualAddress + ((next->NoOfPage + mmap->NoOfPage) << 12),
						.next = mmap->next ? mmap->next : &mmap[1]
					};
					used_count = 2;
				}
				mmap->next = next;
				break;
			}
		}
		if (isLast)
		{
			break;
		}
		else
		{
			mmap = mmap->next ? mmap->next : &mmap[1];
		}
	}
	return used_count;
}

void gotoKernel(KERNEL_ENTER kernel, Table *table, bool isE801, UINT64 MemorySize, UINT64 cpuExInfo, ARD *ards, UINT32 ard_count)
{
	table->mmap = (MMAP *) ((void *)table + sizeof(Table));
	if (isE801)
	{
		bool isHigher = MemorySize >= 0x9f000;
		table->mmap[0] = (MMAP){
			.type = MAP_TYPE_FREE_MEMORY,
			.NoOfPage = (isHigher ?  0x9f000 : MemorySize) / 0x1000,
			.PhysicalAddress = 0x00
		};
		table->mmap[1] = (MMAP){
			.type = MAP_TYPE_UNKNOW,
			.NoOfPage = 1,
			.PhysicalAddress = (void *) 0x9f000
		};
		table->mmap[2] = (MMAP){
			.type = MAP_TYPE_UNKNOW,
			.NoOfPage =  0x10000 / 0x1000,
			.PhysicalAddress = (void *) 0xf0000,
			.isLast = !isHigher
		};
		if (isHigher)
			table->mmap[3] = (MMAP){
				.type = MAP_TYPE_FREE_MEMORY,
				.NoOfPage = (MemorySize - 0x9fc00) / 0x1000,
				.PhysicalAddress = (void *) 0x100000,
				.isLast = true
			};
		table->map_count = 4 + isHigher ? 1 : 0;
	}
	else
	{
		for (int i = 0; i < ard_count; ++i)
		{
			switch (ards[i].type)
			{
				case Memory:
					table->mmap[i] = (MMAP){
						.type = MAP_TYPE_FREE_MEMORY,
						.NoOfPage = ards[i].Length / 0x1000,
						.PhysicalAddress = (void *) (ards[i].BaseAddr & 0xfffffffffffff000)
					};
					break;
				case ACPIMemory:
					table->mmap[i] = (MMAP){
						.type = MAP_TYPE_ACPI,
						.NoOfPage = ards[i].Length / 0x1000 + ards[i].Length % 0x1000 ? 1 : 0,
						.PhysicalAddress = (void *) (ards[i].BaseAddr & 0xfffffffffffff000)
					};
					break;
				case Reserved:
				default:
					table->mmap[i] = (MMAP){
						.type = MAP_TYPE_UNKNOW,
						.NoOfPage = ards[i].Length / 0x1000 + ards->Length % 0x1000 ? 1 : 0,
						.PhysicalAddress = (void *) (ards[i].BaseAddr & 0xfffffffffffff000)
					};
			}
		}
		table->map_count = ard_count;
		table->mmap[table->map_count - 1].isLast = true;
	}
	Elf64_Ehdr *head = (Elf64_Ehdr *) 0x200000;
	Elf64_Phdr *phead = (Elf64_Phdr *) ((unsigned long long) head + head->e_phoff);
	MMAP *next_map_at = &table->mmap[table->map_count];
	for (int i = 0; i < head->e_phnum; ++i)
	{
		if (phead[i].p_type == PT_LOAD && (phead[i].p_vaddr & 0xffff800000000000))
		{
			UINT8 type = phead[i].p_flags & PF_X ? MAP_TYPE_OSCODE : MAP_TYPE_OSDATA;
			UINT8 flags = (phead[i].p_flags & PF_R ? MAP_FLAG_READ : 0) | (phead[i].p_flags & PF_W ? MAP_FLAG_WRITE : 0) | (phead[i].p_flags & PF_X ? MAP_FLAG_EXECTION : 0);
			table->map_count += add_map_at(table->mmap, (void *)((UINT64)head + phead[i].p_offset), (void *)phead[i].p_vaddr, type, flags, phead[i].p_memsz / 0x1000 + phead[i].p_memsz % 0x1000 ? 1 : 0, next_map_at);
			next_map_at = &table->mmap[table->map_count];
		}
	}
	table->cpuExInfo = cpuExInfo;
	table->gconfigre.status = false;
	table->gconfigre.text.height = 25;
	table->gconfigre.text.width = 80;
	table->gconfigre.text.screen = (Word *) 0xffff8000000b8000;
	kernel(table);
}
