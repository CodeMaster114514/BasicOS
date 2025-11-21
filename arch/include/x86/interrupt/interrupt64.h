#ifndef INTERUPT64_H
#define INTERRUPT64_H

#include "btype.h"

#define DP 0x80 // Descript On
#define DPL0 0
#define DPL1 0x20
#define DPL2 0x40
#define DPL3 0x60
#define InterruptGate 0xe
#define TrapGate 0xf

typedef struct
{
	UINT16 offset0; // offset bit 0 to 15
	UINT16 SegmentSelect;
	UINT8 ist; // bit 0 to 2 are used 
	UINT8 types;
	UINT16 offset1; // offset bit 16 to 31
	UINT32 offset2; // offset bit 32 to 63
	UINT32 zero;
} __attribute__((packed)) IDT;

typedef struct
{
	UINT16 size;
	IDT *address;
} __attribute__((packed)) IDTR;

#endif