#ifndef VIDEO_H
#define VIDEO_H

#include "btype.h"
#include "config.h"

#if VGA

#include "gpu/vga.h"

#endif

void InitVideo(GraphicConfigure *gconfigure);
void putc(char c, UINT16 color);

#endif