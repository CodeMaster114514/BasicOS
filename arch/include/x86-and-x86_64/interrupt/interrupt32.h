#ifndef INTERRUPT32_H
#define INTERRUPT32_H

#include "btype.h"

typedef struct
{
	UINT16 size;
	UINT32 address;
} __attribute__((packed)) IDTR;

#endif