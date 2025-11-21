#ifndef MMAPS_H
#define MMAPS_H

void addMMAPAfter(MMAP *mmap, MMAP *target);
void addMMAPBefore(MMAP *mmap, MMAP *target);
MMAP *findAFreeMMAP();
MMAP *findAFreeMMAPWithHave2MBPage();
MMAP *findThePreviousMMAPOf(MMAP *mmap);
void checkNextWrite();
MMAP *findA_NULL_MMAP();
int HowManyCanPut();

#endif