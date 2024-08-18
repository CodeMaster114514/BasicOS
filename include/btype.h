#ifndef BTYPE_H
#define BTYPE_H

#include "config.h"

typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int UINT32;
typedef unsigned long long UINT64;

typedef unsigned char bool;

#define true 1
#define false 0

enum
{
    MAP_TYPE_FREE_MEMORY,
    MAP_TYPE_OSCODE,
    MAP_TYPE_OSDATA,
    MAP_TYPE_APIC,
    MAP_TYPE_ACPI,
    MAP_TYPE_OEM,
    MAP_TYPE_USER_CODE,
    MAP_TYPE_USER_DATA,
    MAP_TYPE_HARDWARE,
    MAP_TYPE_UNKNOW
};

enum
{
    MAP_FLAG_READ = 0b00000001,
    MAP_FLAG_WRITE = 0b00000010,
    MAP_FLAG_EXECTION = 0b00000100
};

typedef struct mmap MMAP;

struct mmap
{
    UINT8 type;
    UINT8 flags;
    UINT64 NoOfPage;
    UINT64 PhysicalAddress;
    void *VirtualAddress;
    bool isLast;
    MMAP *next;
};

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

typedef struct
{
    int LinearAddrSize;
    int PhysicalAddrSize;
    MMAP *mmap;
    UINT64 map_count;
    GraphicConfigure gconfigre;
} Table;

#endif
