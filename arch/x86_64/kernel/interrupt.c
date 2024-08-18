#include "btype.h"

#include "x86-and-x86_64/interrupt/interrupt64.h"
#include "interrupt.h"
#include "memory.h"
#include "x86-and-x86_64/io/io64.h"

#define interrupt_frame void *

IDTR getIDTR()
{
	IDTR idtr;
	asm(
		"sidt %0\n\t"
		:"=m"(idtr)
	);
	return idtr;
}

void setIDTR(IDTR idtr)
{
	asm(
		"lidt %0\n\t"
		:
		:"m"(idtr)
	);
}

__attribute__((interrupt))
void CommonInterrupt(interrupt_frame frame)
{
	return;
}

void setInterrupt(UINT16 SegmentSelect, void *address, UINT8 ist, UINT8 types, UINT8 InterruptNumber)
{
	IDTR idtr = getIDTR();
	IDT* idts = idtr.address;
	IDT* target = &idts[InterruptNumber];
	*target = (IDT){
		.SegmentSelect = SegmentSelect,
		.offset0 = (UINT16) address,
		.offset1 = (UINT16) ((UINT64)address >> 16),
		.offset2 = (UINT32) ((UINT64)address >> 32),
		.types = types,
		.ist = ist,
		.zero = 0
	};
	setIDTR(idtr);
}

void InitInterrupt()
{
	IDTR idtr = (IDTR){
		.address = allocA4KBPage(MAP_TYPE_OSDATA, MAP_FLAG_READ | MAP_FLAG_WRITE),
		.size = 4095
	};
	setIDTR(idtr);
	for (int i = 0; i < 20; ++i)
	{
		setInterrupt(getCS(), CommonInterrupt, 0, DP | DPL0 | InterruptGate, i);
	}
	for (int i = 20; i < 32; ++i)
	{
		setInterrupt(0, 0, 0, 0, 0);
	}
	for (int i = 32; i < 256; ++i)
	{
		setInterrupt(getCS(), CommonInterrupt, 0, DP | DPL0 | InterruptGate, i);
	}
	puts("%d\n%d\n", sizeof(IDT), sizeof(IDTR));
	puts("%p\n", idtr.address);
	openInterrupt();
}