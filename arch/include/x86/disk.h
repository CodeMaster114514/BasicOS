#ifndef X86_DISK_H
#define X86_DISK_H

#include "btype.h"

int read_boot_disk(UINT32 LBA, UINT32 count, void *out);

#endif
