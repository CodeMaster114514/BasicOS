#include "x86/boot/memory.h"
#include "btype.h"
#include "elf_kernel/elf.h"

PML4E *PML4es = (void *)0x100000;
void *next = (void *)0x104000;
void *free_start = 0x20000;
int free_count = 16;

void addVirtualMemory(UINT64 PhysicalAddress, UINT64 VirtualAddress, UINT32 PageCount, UINT32 flags_low, UINT32 flags_high, UINT32 type)
{
    UINT32 pml4Index = (VirtualAddress >> 39) & 0x1FF;
    UINT32 pdptIndex = (VirtualAddress >> 30) & 0x1FF;
    UINT32 pdIndex = (VirtualAddress >> 21) & 0x1FF;
    UINT32 ptIndex = (VirtualAddress >> 12) & 0x1FF;

    PML4E pml4e = PML4es[pml4Index];
    PDPTE *pdptes;
    PDE *pdes;
    PTE *ptes;
    if (!(pml4e.base & P))
    {
        pml4e.base = (UINT32)next | P | R_W;
        pml4e.high = 0;
        PML4es[pml4Index] = pml4e;
        next += 0x1000;
    }

    pdptes = (void *)(pml4e.base & 0xfffff000);

    if (type == PageSize1GB)
    {
        for (int i = 0; i < PageCount; ++i)
        {
            pdptes[pdptIndex + i].base = ((UINT32)PhysicalAddress) | P | PS1GB | flags_low;
            pdptes[pdptIndex + i].high = (UINT32)(PhysicalAddress >> 32) | flags_high;
            PhysicalAddress += 0x40000000;
        }
        goto end;
    }
    else if (!(pdptes[pdptIndex].base & P))
    {
        pdptes[pdptIndex].base = (UINT32)PhysicalAddress | P | R_W;
        pdptes[pdptIndex].high = 0;
        next += 0x1000;
    }

    pdes = (void *)(pdptes[pdptIndex].base & 0xfffff000);
    if (type == PageSize2MB)
    {
        for (int i = 0; i < PageCount; ++i)
        {
            pdes[pdIndex + i].base = ((UINT32)PhysicalAddress) | P | PS2MB | flags_low;
            pdes[pdIndex + i].high = (UINT32)(PhysicalAddress >> 32) | flags_high;
            PhysicalAddress += 0x200000;
        }
        goto end;
    }
    else if (!(pdes[pdIndex].base & P))
    {
        pdes[pdIndex].base = (UINT32)next | P | R_W;
        pdes[pdIndex].high = 0;
        next += 0x1000;
    }

    ptes = (void *)(pdes[pdIndex].base & 0xfffff000);

    if (type == PageSize4KB)
    {
        for (int i = 0; i < PageCount; ++i)
        {
            ptes[ptIndex + i].base = ((UINT32)PhysicalAddress) | P | flags_low;
            ptes[ptIndex + i].high = (UINT32)(PhysicalAddress >> 32) | flags_high;
            PhysicalAddress += 0x1000;
        }
    }
end:
    asm(
        "mov eax, %0\n\t"
        "mov cr3, eax\n\t"
        :
        : "m"(PML4es)
        : "eax");
    return;
}

void setVirtualMemory()
{
    for (int i = 0; i < 512; ++i)
    {
        PML4es[i].base = 0;
        PML4es[i].high = 0;
    }
    PML4es[511].base = ((UINT32)PML4es) | P | R_W;
    PML4es[511].high = 0;
    PML4es[0].base = 0x101000 | P | R_W;
    PML4es[0].high = 0;
    PML4es[256] = PML4es[0];
    PDPTE *pdpt = (void *)0x101000;
    for (int i = 0; i < 512; ++i)
    {

        pdpt[i].base = 0;
        pdpt[i].high = 0;
    }
    pdpt[0].base = 0x102000 | P | R_W;
    pdpt[0].high = 0;
    PDE *pd = (void *)0x102000;
    for (int i = 0; i < 512; ++i)
    {
        pd[i].base = 0;
        pd[i].high = 0;
    }
    pd[0].base = 0 | P | R_W | PS2MB;
    pd[0].high = 0;

    asm(
        "mov eax, %0\n\t"
        "mov cr3, eax\n\t"
        "mov eax, cr4\n\t"
        "or eax, 0x10\n\t"
        "mov cr4, eax\n\t"
        :
        : "m"(PML4es)
        : "eax");
}

UINT64 add_map_at(MMAP *mmap, UINT64 PhysicalAddress, UINT8 type, UINT8 flags, UINT64 NoOfPage, MMAP *next)
{
    UINT64 used_count;
    MMAP *mmap_backup = mmap;
    bool isLast = false;
    while (1)
    {
        isLast = mmap->isLast;
        if (mmap->PhysicalAddress <= (UINT64)PhysicalAddress && mmap->PhysicalAddress + (mmap->NoOfPage << 12) > (UINT64)PhysicalAddress)
        {
            if ((UINT64)PhysicalAddress == mmap->PhysicalAddress)
            {
                if (mmap->NoOfPage > NoOfPage)
                {
                    *next = *mmap;
                    next->NoOfPage -= NoOfPage;
                    next->PhysicalAddress += NoOfPage << 12;
                    // next->VirtualAddress += NoOfPage << 12;
                    next->next = mmap->next ? mmap->next : &mmap[1];
                    used_count = 1;
                }
                mmap->type = type;
                mmap->flags = flags;
                // mmap->VirtualAddress = VirtualAddress;
                mmap->NoOfPage = NoOfPage;
                mmap->isLast = false;
                mmap->next = next;
                break;
            }
            else
            {
                UINT64 NoOfPageCache = mmap->NoOfPage;
                mmap->NoOfPage = (UINT64)(PhysicalAddress - mmap->PhysicalAddress) >> 12;
                bool NeedThirdMap = NoOfPage < NoOfPageCache && NoOfPageCache - mmap->NoOfPage - NoOfPage > 0;
                *next = (MMAP){
                    .type = type,
                    .flags = flags,
                    .NoOfPage = NoOfPage,
                    .isLast = (!NeedThirdMap) && isLast,
                    .PhysicalAddress = (UINT64)PhysicalAddress,
                    //.VirtualAddress = VirtualAddress,
                    .next = NeedThirdMap ? 0 : (mmap->next ? mmap->next : &mmap[1])};
                used_count = 1;
                if (NeedThirdMap)
                {
                    next[1] = (MMAP){
                        .type = mmap->type,
                        .flags = mmap->flags,
                        .isLast = mmap->isLast,
                        .NoOfPage = NoOfPageCache - mmap->NoOfPage - NoOfPage,
                        .PhysicalAddress = next->PhysicalAddress + (next->NoOfPage << 12),
                        //.VirtualAddress = mmap->VirtualAddress + ((next->NoOfPage + mmap->NoOfPage) << 12),
                        .next = mmap->next ? mmap->next : &mmap[1]};
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

void ModifyMMAP(MMAP *mmap, ARD *ards, UINT32 count, Elf64_Ehdr *ehdr)
{
    for (int i = 0; i < 4096 / sizeof(MMAP); ++i)
    {
        UINT64 *data = (UINT64 *)mmap;
        data[i] = 0;
    }
    for (int i = 0; i < count; ++i)
    {
        switch (ards[i].type)
        {
        case Memory:
            mmap[i].type = MAP_TYPE_FREE_MEMORY;
            mmap[i].flags = 0;
            mmap[i].NoOfPage = ards[i].Length / 0x1000 + (ards[i].Length % 0x1000 ? 1 : 0);
            mmap[i].PhysicalAddress = (ards[i].BaseAddr & 0xfffffffffffff000);
            mmap[i].isLast = false;
            mmap[i].next = 0;
            break;
        case ACPIMemory:
            mmap[i].type = MAP_TYPE_ACPI;
            mmap[i].flags = MAP_FLAG_READ | MAP_FLAG_WRITE;
            mmap[i].NoOfPage = ards[i].Length / 0x1000 + (ards[i].Length % 0x1000 ? 1 : 0);
            mmap[i].PhysicalAddress = (ards[i].BaseAddr & 0xfffffffffffff000);
            mmap[i].isLast = false;
            mmap[i].next = 0;
            break;
        default:
            mmap[i].type = MAP_TYPE_UNKNOW;
            mmap[i].flags = MAP_FLAG_READ;
            mmap[i].NoOfPage = ards[i].Length / 0x1000 + (ards[i].Length % 0x1000 ? 1 : 0);
            mmap[i].PhysicalAddress = (ards[i].BaseAddr & 0xfffffffffffff000);
            mmap[i].isLast = false;
            mmap[i].next = 0;
        }
    }
    mmap[count - 1].isLast = true;
}
