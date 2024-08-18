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
	if (!num) return "0";
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

const char *PointerToString(void *addr)
{
	UINT64 addr_num = (UINT64) addr; //将地址信息转化为数字，以便计算
	if (!addr_num) return "0x0";
	int string_index = 0;
	while (addr_num != 0)
	{
		int remainer = addr_num % 0xf;
		if (remainer < 10)
		{
			NumberString0[string_index] = remainer + 0x30;
		}
		else
		{
			NumberString0[string_index] = remainer - 10 + 'a';
		}
		addr_num /= 0xf;
		string_index++;
	}
	--string_index;
	int i = 2;
	NumberString1[0] = '0';
	NumberString1[1] = 'x';
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
	int count = 0;//str索引
	int str_len = 0;
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
				str_len += 2;
				break;
			
			case 's':
				puts(va_arg(list, const char *));
				count += 2;
				str_len += 2;
				break;
			
			case 'p':
				puts(PointerToString(va_arg(list, void *)));
				count += 2;
				str_len += 2;
			
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
			str_len++;
		}
	}
	
}