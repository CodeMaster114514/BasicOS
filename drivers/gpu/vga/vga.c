#include "gpu/vga.h"
#include "btype.h"
#include "x86/io/io32.h"

UINT16 getCursor()
{
	io_out8(0x3d4, 0x0e);
	UINT16 cursor = io_in8(0x3d5) << 8;
	io_out8(0x3d4, 0x0f);
	cursor |= io_in8(0x3d5);
	return cursor;
}

void setCursor(UINT16 cursor)
{
	io_out8(0x3d4, 0x0f);
	io_out8(0x3d5, cursor & 0x00ff);
	io_out8(0x3d4, 0x0e);
	io_out8(0x3d5, cursor >> 8);
}