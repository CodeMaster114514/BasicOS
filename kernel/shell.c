#include "btype.h"
#include "memory.h"
#include "video.h"
#include <stdarg.h>

char *cache = 0;

void InitShell()
{
	// alloc(1);
}

char NumberString0[64] = {0};//存储反向的字符串
char NumberString1[64] = {0};//存储正向的字符串

const char *NumToString(long long num)
{
	int string_index = 0;
	while (num != 0)
	{
		NumberString0[string_index] = num % 10 + 0x30;
		num /= 10;
		string_index++;
	}
	--string_index;
	int i = 0;
	while(string_index >= 0)
	{
		NumberString1[i] = NumberString0[string_index];
		i++;
		string_index--;
	}
	NumberString1[i] = '\0';
	return NumberString1;
}

int puts(const char *str, ...)
{
	va_list list;
	int count = 0;//计算str长度同时作为str索引
	va_start(list, str);
	while (str[count] != '\0')
	{
		if (str[count] == '%')
		{
			switch (str[count + 1])
			{
			case 'd':
				puts(NumToString(va_arg(list, int)));
				count += 2;
				break;
			
			case 's':
				puts(va_arg(list, const char *));
				count += 2;
				break;
			
			default:
				goto normal;
				break;
			}
		}
		else
		{
		normal:
			putc(str[count], 0xffff);
			count++;
		}
	}
	
}