#include "x86/boot/boot.h"
#include "acpi/ARDS.h"
#include "btype.h"
#include "x86/boot/memory.h"
#include "x86/video.h"
#include "x86/fat.h"
#include "elf_kernel/elf.h"

bool isSupport64bitsProtectionMode();
void enter64ProtectMode(bool enableNX);

typedef struct
{
    Word *screen;
	UINT32 for_64;
    UINT32 width;
    UINT32 height;
} __attribute__((packed)) TextMode32;

typedef struct
{
    bool status;//文本模式为false
    TextMode32 text;
} __attribute__((packed)) GraphicConfigure32;

typedef struct
{
	int LinearAddrSize;
    int PhysicalAddrSize;
    MMAP *mmap;
	UINT32 for_64;
    UINT64 map_count;
    GraphicConfigure32 gconfigre;
} Table32;

UINT64 entry64;
UINT64 canonial = 0xffff800000000000;
Table32 table;

void call64();

UINT32 getPageCount(UINT64 address, UINT64 size);

bool isSupportNX();

int getLinearAddrSize()
{
	int size = 0;
	asm(
		"mov eax, 0x80000008\n\t"
		"cpuid\n\t"
		"movzx eax,ah\n\t"
		"mov %0, eax\n\t"
		: "=m"(size)
		:
		: "eax", "ebx", "ecx", "edx"
	);
	return size;
}

int getPhysicalAddrSize()
{
    int size = 0;
    asm(
        "mov eax, 0x80000008\n\t"
        "cpuid\n\t"
        "movzx eax, al\n\t"
		"mov %0, eax\n\t"
        :"=m"(size)
        :
        :"eax","ebx","ecx","edx"
    );
	return size;
}

void _start()
{
	int *ARDCount = (int *)0x9000;
	ARD *ards = (ARD *)0x7e00;
	asm(
		"mov ax, 0x0008\n\t"
		"mov ds, ax\n\t"
		"mov es, ax\n\t"
		"mov fs, ax\n\t"
		"mov gs, ax\n\t"
	);
	table.mmap = (void *)0x30000;
	table.for_64 = 0xffff8000;
	struct {
		UINT32 base;
		UINT16 segment;
	} __attribute__((packed)) entry;
	puts("The System is Booting now.\n");
    if (!isSupport64bitsProtectionMode())
    {
        puts("The CPU haven't supported 64 bits protection mode.");
        goto end;
    }
    bool nx = isSupportNX();
    if (!nx)
    {
        puts("warn: the cpu haven't supported NX bit, might make the computer unsafe.\n\t");
    }
	ReadResult result = readFile("kernel", (void *)0x200000);
	if (!result.status)
	{
		puts("Can't find kernel file.");
		goto end;
	}
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)0x200000;
	Elf64_Phdr *phdr = (Elf64_Phdr *)(ehdr->e_phoff + (void *)ehdr);
	entry64 = (UINT64)ehdr->e_entry;
	entry.base = (UINT32) call64;
	entry.segment = 0x18;
    setVirtualMemory();
	ModifyMMAP(table.mmap, ards, *ARDCount, ehdr);
	table.map_count = *ARDCount;
	table.LinearAddrSize = getLinearAddrSize();
	table.PhysicalAddrSize = getPhysicalAddrSize();
	table.gconfigre.status = false;
	table.gconfigre.text.screen = (Word *)0xb8000;
	table.gconfigre.text.height = 0xffff8000;
	table.gconfigre.text.width = 80;
	table.gconfigre.text.height = 25;
	for (int i = 0; i < ehdr->e_phnum; i++)
	{
		if (phdr[i].p_type == PT_LOAD)
		{
			UINT32 type = PageSize4KB;
			UINT32 flags_low = P;
			UINT32 flags_high = 0;
			UINT32 PageCount = getPageCount(phdr[i].p_vaddr, phdr[i].p_memsz);
			UINT32 MMAP_FALGS = MAP_FLAG_READ;
			UINT32 MMAP_TYPE = MAP_TYPE_OSCODE;
			if (phdr[i].p_flags & PF_W)
			{
				MMAP_FALGS |= MAP_FLAG_WRITE;
				MMAP_TYPE |= MAP_TYPE_OSDATA;
				flags_low |= R_W;
			}
			if (!(phdr[i].p_flags & PF_X))
			{
				MMAP_FALGS &= ~MAP_FLAG_EXECTION;

				if (nx)
					flags_high |= XD >> 32;
			}
			table.map_count += add_map_at(table.mmap, phdr[i].p_offset + 0x200000, MMAP_TYPE, MMAP_FALGS, PageCount, &table.mmap[table.map_count]);
			if (PageCount > 512)
			{
				PageCount = (phdr[i].p_memsz + 0x1fffff) / 0x200000;
				type = PageSize2MB;
			}
			if (PageCount > 512)
			{
			    PageCount = (phdr[i].p_memsz + 0x3fffffff) / 0x40000000;
			    type = PageSize1GB;
			}
			addVirtualMemory((phdr[i].p_offset + 0x200000) & 0xfffffffffffff000, phdr[i].p_vaddr, PageCount, flags_low, flags_high, type);
		}
	}
    enter64ProtectMode(nx);
	asm(
		"xor edi, edi\n\t"
		"lea edi, %2\n\t"
		"lea ebx, %1\n\t"
		"lcall %0\n\t"
		:
		: "m"(entry), "m"(entry64), "m"(table)
		: "eax", "ebx", "ecx", "edx"
	);
end:
	for (;;)
	{
		asm(
			"hlt\n\t");
	}
}

void enter64ProtectMode(bool enableNX)
{
	UINT32 enbits = 1 << 8; // Enable PAE
	if (enableNX)
	{
		enbits |= 1 << 11; // Enable NX
	}
    asm(
        "mov eax, cr4\n\t"
        "or eax, 0b100000\n\t"
        "mov cr4, eax\n\t"
        "mov ecx, 0xc0000080\n\t"
        "rdmsr\n\t"
        "or eax, %0\n\t"
        "wrmsr\n\r"
        "mov eax, cr0\n\t"
        "or eax, 0x80000000\n\t"
        "mov cr0, eax\n\t"
        :
        :"m"(enbits)
        :"eax", "ebx", "ecx", "edx"
    );
}

bool isSupport64bitsProtectionMode()
{
	bool supported = false;
	asm(
		"mov eax, 0x80000000\n\t"
		"cpuid\n\t"
		"cmp eax, 0x80000000\n\t"
		"jbe .end\n\t"
		"mov eax, 0x80000001\n\t"
		"cpuid\n\t"
		"bt edx, 29\n\t"
		"jnc .end\n\t"
		"mov %0, 1\n\t"
		".end:\n\t"
		: "=m"(supported)
		:
		: "eax", "ebx", "ecx", "edx"
	);
	return supported;
}

bool isSupportNX()
{
	bool supported = false;
	asm(
		"mov eax, 0x80000001\n\t"
		"cpuid\n\t"
		"bt edx, 20\n\t"
		"jnc .end0\n\t"
		"mov %0, 1\n\t"
		".end0:\n\t"
		: "=m"(supported)
		:
		: "eax", "ebx", "ecx", "edx"
	);
	return supported;
}

asm(
	".global call64\n\t"
	".code64\n\t"
	"call64:\n\t"
	"xor rax, rax\n\t"
	"mov eax, 0xffff8000\n\t"
	"shl rax, 32\n\t"
	"or rdi, rax\n\t"
	"mov rbx, [rbx]\n\t"
	"mov rsp, 0x7c00\n\t"
	"call rbx\n\t"
	".end1:\n\t"
	"hlt\n\t"
	"jmp .end1\n\t"
	"ret\n\t"
	".code32\n\t"
);

UINT32 getPageCount(UINT64 address, UINT64 size)
{
	UINT64 startPage = address & 0xfffffffffffff000;
	UINT64 endPage = (address + size - 1) & 0xfffffffffffff000;
	return ((endPage - startPage) >> 12) + 1;
}
