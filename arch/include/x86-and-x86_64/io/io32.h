#ifndef IO32_H
#define IO32_H

#include "btype.h"

static inline UINT8 io_in8(UINT16 port)
{
	UINT8 data;
	asm(
		"mov dx, %1\n\t"
		"in al, dx\n\t"
		"mov %0, al\n\t"
		:"=m"(data)
		:"m"(port)
		:"eax", "edx"
	);
	return data;
}

static inline void io_out8(UINT16 port, UINT8 data)
{
	asm(
		"mov dx, %1\n\t"
		"mov al, %0\n\t"
		"out dx, al\n\t"
		:
		:"m"(data), "m"(port)
		:"eax", "edx"
	);
}

#endif