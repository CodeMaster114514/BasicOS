#ifndef MBR_H
#define MBR_H

#include "btype.h"

typedef struct
{
    UINT8 BootIndicator;
    UINT8 StartHeada;
    UINT8 StartSector;
    UINT8 StartTrack;
    UINT8 OSType;
    UINT8 EndHead;
    UINT8 EndSector;
    UINT8 ENDTrack;
    UINT32 StartLBA;
    UINT32 EndLBA;
} __attribute__((packed)) MBR_PARTITION;

typedef struct
{
    UINT8 code[440];
    UINT8 UniqueMbrSignature[4];
    UINT8 Unknow[2];
    MBR_PARTITION partition[4];
    UINT8 Signture[2];
} __attribute__((packed)) MBR;

#endif
