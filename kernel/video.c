#include "video.h"

TextMode *text;

void InitVideo(GraphicConfigure *gconfigure)
{
	if (gconfigure->status)
	{}
	else
	{
		text = &gconfigure->text;
	}
}

void copyWord(UINT16 target, UINT16 source, UINT16 count)
{
	for (UINT16 i = 0; i < count; ++i)
	{
		text->screen[target] = text->screen[source];
	}
}

void putc(char c, UINT16 color)
{
	UINT16 cursor = getCursor();
	if (c == '\n')
	{
		cursor = cursor - cursor % 80 + 80;
		goto roll;
	}
	if (c == '\r')
	{
		cursor = cursor - cursor % 80;
		goto set;
	}
	if (color <= 0x0100)
	{
		text->screen[cursor].color = color & 0x00ff;
	}
	text->screen[cursor].word = c;
	cursor++;
roll:
	if (cursor >= 2000)
	{
		copyWord(0, 80, 1920);
		cursor = 1920;
	}
set:
	setCursor(cursor);
}