#ifndef FAT_H
#define FAT_H

#include "btype.h"

enum
{
    FAT12,
    FAT16,
    FAT32,
    exFAT
};

typedef struct
{
    UINT8 code[3];
    char name[8];
    UINT16 SectorSize;
    UINT8 SecPerClu; //每簇有几个扇区
    UINT16 RsvdSecCount; //保留扇区数
    UINT8 NumsFATs;
    UINT16 RootEntCnt;
    UINT16 SecCnt16;
    char DriverDescriptor;
    UINT16 FATTableSize;
    UINT16 LogicSecPerTra; // 逻辑每磁道扇区数
    UINT16 LogicHeadCnt; // 逻辑磁头数
    UINT32 HideSecCnt; // 隐藏扇区数
    UINT32 SecCnt32;
} __attribute__((packed)) FAT_BSB;

typedef struct
{
    UINT8 BIOS_DriverNumber;
    UINT8 Reserved0;
    UINT8 BootSignature;
    UINT32 VolumeId;
    char VolumeName[11];
    char FileSystemType[8];
    UINT8 codes[448];
    UINT8 Signture[2];
} __attribute__((packed)) exFAT_BSB12_16;

typedef struct
{
    UINT32 TableSize;
    UINT16 ExtFlag;
    UINT16 FATVersion;
    UINT32 RootCluster;
    UINT16 FAT_info;
    UINT16 BackupSector;
    UINT8 Reserved0[12];
    UINT8 BIOS_DriverNumber;
    UINT8 Reserved1;
    UINT8 BootSignature;
    UINT32 VolumeId;
    char VolumeName[8];
    char FileSystemType[8];
    UINT8 codes[420];
    UINT8 Signture[2];
} __attribute__((packed)) exFAT_BSB32;

typedef struct
{
    char name[11];
    UINT8 flags;
    UINT8 Reserved;
    UINT8 Millisecond;
    UINT16 time;
    UINT16 date;
    UINT16 LastVisitTime;
    UINT16 ClusterCodeHigh;
    UINT16 LastChangeTime;
    UINT16 LastChangeDate;
    UINT16 ClusterCodeLow;
    UINT32 Length;
} __attribute__((packed)) FAT32_ENTEY;

typedef struct
{
    UINT32 Signature0;
    UINT8 Reserved0[480];
    UINT32 Signature1;
    UINT32 FreeCount;
    UINT32 LastFreeCluster;
    UINT8 Reserved1[12];
    UINT16 Signture2;
} __attribute__((packed)) FS_info;

#endif
