#include "x86/interrupt/interrupt32.h"

void __attribute__((interrupt)) commandInterruptFunction(InterruptFrame *frame)
{
    return;
}


bool setInterrupt()
{
    IDTEntry* idt = (void *) 0x8000;
    for (int i = 0; i < 256; i++)
    {
        idt[i].offset_low = (UINT16)((UINT32)commandInterruptFunction & 0xFFFF);
        idt[i].selector = 0x10; // Kernel code segment selector
        idt[i].zero = 0;
        idt[i].type_attr = 0x8E; // Present, DPL=0, Type=INT
        idt[i].offset_high = (UINT16)(((UINT32)commandInterruptFunction >> 16) & 0xFFFF);
    }
    IDTR idtr;
    idtr.size = sizeof(IDTEntry) * 256 - 1;
    idtr.address = (UINT32)&idt;
    asm(
        "lidt %0\n\t"
        "sti\n\t"
        :
        : "m"(idtr)
    );
}
