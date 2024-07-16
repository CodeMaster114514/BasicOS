#ifndef BTYPE_H
#define BTYPE_H

#include "config.h"

typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int UINT32;
typedef unsigned long long UINT64;

typedef unsigned char bool;

enum
{
    MAP_FREE_MEMORY,
    MAP_TYPE_OSCODE,
    MAP_TYPE_OSDATA,
    MAP_TYPE_APIC,
    MAP_TYPE_ACPI,
    MAP_TYPE_USER_CODE,
    MAP_TYPE_USER_DATA
};

enum
{
    MAP_FLAG_READ = 0b00000001,
    MAP_FLAG_WRITE = 0b00000010,
    MAP_FLAG_EXECTION = 0b00000100
};

typedef struct
{
    UINT8 type;
    UINT8 flags;
    UINT32 NoOfPage;
    void *PhysicalAddress;
    void *VirtualAddress;
} __attribute__((packed)) MMAP;

typedef struct
{
    char word;
    UINT8 color;
} Word;

typedef struct
{
    Word *screen;
    UINT32 width;
    UINT32 height;
} __attribute__((packed)) TextMode;

typedef struct
{
    bool status;//文本模式为false
    TextMode text;
} __attribute__((packed)) GraphicConfigure;

#if X86_64

typedef struct
{
} MProcess;

#endif

typedef struct
{
    MMAP *mmap;
    UINT64 map_count;
    GraphicConfigure gconfigre;
#if X86_64
    UINT64 cpuExInfo;
    MProcess mprocess;
#endif
} Table;

#endif
