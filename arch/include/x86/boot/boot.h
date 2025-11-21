#ifndef BOOT_H
#define BOOT_H

#define BootAddress 0x20000
#define CoreDataStart 0x8000

#ifdef ASM
    .extern loder_entry
#else
void loder_entry();
#endif

#endif
