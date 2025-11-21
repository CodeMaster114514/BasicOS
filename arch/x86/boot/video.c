#include "x86/video.h"
#include "gpu/vga.h"
#include <arg.h>

void putc(char c)
{
    short cursor = getCursor();
    Word *screen = (void *)0xb8000;
    switch (c)
    {
    case '\n':
        cursor = cursor - cursor % 80 + 80;
        break;
    case '\r':
        cursor = cursor - cursor % 80;
        break;

    default:

        if (cursor >= 80 * 25)
        {
            for (int i = 0; i < 80 * 24; ++i)
            {
                screen[i].word = screen[i + 80].word;
            }
            for (int i = 80 * 24; i < 80 * 25; ++i)
            {
                screen[i].word = 0x00;
                screen[i].color = 0x0f;
            }
            cursor -= 80;
        }
        screen[cursor++].word = c;
        break;
    }
    setCursor(cursor);
}

int puts(const char *str, ...)
{
    int i = 0, count = 0;
    va_list args;
    va_start(args, str);
    while (str[i] != '\0')
    {
        if (str[i] == '\\')
        {
            switch (str[i + 1])
            {
            case 's':
                count += puts(va_arg(args, const char *));
                break;
            }
        }
        else
        {
            putc(str[i]);
            ++count;
        }
        ++i;
    }
    va_end(args);
    return count;
}
