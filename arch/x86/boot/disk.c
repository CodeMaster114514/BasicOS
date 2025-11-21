#include "x86/disk.h"
#include "btype.h"
#include "x86/io/io32.h"
#include "x86/video.h"

int read_boot_disk_a_sector(UINT32 LBA, void *out)
{
    io_out8(0x1f2, 1);
    io_out8(0x1f3, LBA >> 0);
    io_out8(0x1f4, LBA >> 8);
    io_out8(0x1f5, LBA >> 16);
    io_out8(0x1f6, LBA >> 20 & 0b1111 | 0b11100000);
    io_out8(0x1f7, 0x20);
    UINT8 status = io_in8(0x1f7);
    while (status & 0b10000000)
    {
        if (status & 0b00000001) goto error;
        status = io_in8(0x1f7);
    }
    for (int i = 0; i < 256; ++i)
    {
        ((UINT16 *)out)[i] = io_in16(0x1f0);
    }
    return 0;
error:
    puts("the ATA disk read error.");
    return 1;
}

int read_boot_disk(UINT32 LBA, UINT32 count, void *out)
{
    for (UINT64 i = 0; i < count; ++i)
    {
        if (read_boot_disk_a_sector(LBA + i, out + i * 512))
            goto error;
    }
    return 0;
error:
    return 1;
}

