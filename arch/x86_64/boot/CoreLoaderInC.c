#include "btype.h"
#include <linux/elf.h>
#include <stdbool.h>
typedef void (*KERNEL_ENTER)(Table *Table);

void gotoKernel(KERNEL_ENTER kernel, Table *table, bool isE801, UINT64 MemorySize, UINT64 cpuExInfo)
{
	table->mmap = (MMAP *) table + sizeof(Table);
	if (isE801)
	{
		table->mmap[0].type = MAP_TYPE_OSDATA;
		table->mmap[0].flags = MAP_FLAG_EXECTION | MAP_FLAG_WRITE | MAP_FLAG_READ;
		table->mmap[0].NoOfPage = 2048 / 4;
		table->mmap[0].PhysicalAddress = 0;
		table->mmap[0].VirtualAddress = (void *) 0xffff800000000000;
		table->mmap[1].type = MAP_TYPE_OSDATA;
		table->mmap[1].flags = MAP_FLAG_EXECTION | MAP_FLAG_WRITE | MAP_FLAG_READ;
		table->mmap[1].PhysicalAddress = (void *) 0x200000;
		table->map_count = 2;
		Elf64_Ehdr *head = (void *) 0xffff800000200000;
		table->mmap[1].VirtualAddress = head;
		Elf64_Phdr *phead = (Elf64_Phdr *) head + head->e_phoff;
		int mmap_offset = 1;
		for (int i = 0; i < head->e_phnum; ++i)
		{
			if (phead[i].p_type == PT_LOAD && phead[i].p_vaddr & 0xffff800000000000)
			{
				if (mmap_offset == 1)
				{
					table->mmap[i + mmap_offset - 1].NoOfPage = phead[i].p_offset / 0x1000;
				}
				table->mmap[1 + mmap_offset].type = phead[i].p_flags & PF_X ? MAP_TYPE_OSCODE : MAP_TYPE_OSDATA;
				table->mmap[1 + mmap_offset].flags = (phead[i].p_flags & PF_W ? MAP_FLAG_WRITE : 0) | (phead[i].p_flags & PF_R ? MAP_FLAG_READ : 0) | (phead[i].p_flags & PF_X ? MAP_FLAG_EXECTION : 0);
				table->mmap[1 + mmap_offset].NoOfPage = phead[i].p_memsz / 0x1000 + phead[i].p_memsz % 0x1000 ? 1 : 0;
				table->mmap[1 + mmap_offset].VirtualAddress = (void *) phead[i].p_vaddr;
				table->mmap[1 + mmap_offset].PhysicalAddress = table->mmap[1].PhysicalAddress + phead[i].p_offset;
				table->map_count++;
				mmap_offset++;
			}
		}
	}
	else
	{}
	table->cpuExInfo = cpuExInfo;
	table->gconfigre.status = false;
	table->gconfigre.text.height = 25;
	table->gconfigre.text.width = 80;
	table->gconfigre.text.screen = (Word *) 0xffff8000000b8000;
	kernel(table);
}
