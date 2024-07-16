#ifndef CPUID_H
#define CPUID_H

typedef enum
{
	FPU    = 0b00000000000000000000000000000001,
	VME    = 0b00000000000000000000000000000010,
	DE     = 0b00000000000000000000000000000100,
	PSE    = 0b00000000000000000000000000001000,
	TSC    = 0b00000000000000000000000000010000,
	MSR    = 0b00000000000000000000000000100000,
	PAE    = 0b00000000000000000000000001000000,
	MCE    = 0b00000000000000000000000010000000,
	CX8    = 0b00000000000000000000000100000000,
	APIC   = 0b00000000000000000000001000000000,
	SEP    = 0b00000000000000000000010000000000,
	MTRR   = 0b00000000000000000000100000000000,
	PGE    = 0b00000000000000000001000000000000,
	MCA    = 0b00000000000000000010000000000000,
	CMOV   = 0b00000000000000000100000000000000,
	PAT    = 0b00000000000000001000000000000000,
	PSE_36 = 0b00000000000000010000000000000000,
	PSN    = 0b00000000000000100000000000000000,
	CLFSN  = 0b00000000000001000000000000000000,
	DS     = 0b00000000000010000000000000000000,
	ACPI   = 0b00000000000100000000000000000000,
	MMX    = 0b00000000001000000000000000000000,
	FXSR   = 0b00000000010000000000000000000000,
	SSE    = 0b00000000100000000000000000000000,
	SSE2   = 0b00000001000000000000000000000000,
	SS     = 0b00000010000000000000000000000000,
	TM     = 0b00100000000000000000000000000000
} name;

#endif
