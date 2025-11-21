#ifndef INTERRUPT32_H
#define INTERRUPT32_H

#include "btype.h"

typedef struct
{
	/* data */
} InterruptFrame;


typedef struct
{
	UINT16 size;
	UINT32 address;
} __attribute__((packed)) IDTR;

typedef struct
{
	UINT16 offset_low;
	UINT16 selector;
	UINT8 zero;
	UINT8 type_attr;
	UINT16 offset_high;
} __attribute__((packed)) IDTEntry;

#endif