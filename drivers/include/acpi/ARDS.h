#ifndef ARDS_H
#define ARDS_H

#include "btype.h"

enum ARDT // Address Range Descriptor Type
{
	Memory = 1,
	Reserved = 2,
	ACPIMemory = 3,
	NVS = 4,
	Unusable = 5,
	Disabled = 6,
	PersistentMemory = 7,
	Unaccepted = 8,
	OEM = 12
};

#define OEM_SMALE_TYPE 0xf00000000
#define OEM_BIG_TYPE 0xffffffff

typedef struct
{
	union
	{
		struct
		{
			UINT32 BaseAddrLow;
			UINT32 BaseAddrHigh;
		};
		UINT64 BaseAddr;
	};
	union
	{
		struct
		{
			UINT32 LengthLow;
			UINT32 LengthHigh;
		};
		UINT64 Length;
	};
	UINT32 type;
	UINT32 EA; // Extended Attributes
} __attribute__((packed)) ARD;// Address Range Descriptor

typedef struct
{
	char Signature[8];
	char Checksum;
	char OEMID[6];
	char revision;
	UINT32 RSDT;
	UINT32 Length;
	UINT64 XSDT;
	char XChecksum;
	char Reserved[3];
} __attribute__((packed)) RSDP;

#endif
