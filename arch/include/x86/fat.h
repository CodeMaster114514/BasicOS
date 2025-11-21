#ifndef X86_FAT_H
#define X86_FAT_H

#include "x86/disk.h"
#include "fat.h"

typedef struct
{
    bool status;
    UINT32 length;
} ReadResult;


ReadResult readFile(const char *path, void* out);

#endif
