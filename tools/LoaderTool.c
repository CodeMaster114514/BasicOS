#include <stdio.h>
#include <stdlib.h>
#include <elf.h>

int main(int args, char *argv[])
{
	FILE *loader, *out;
	int loader_size;
	int entry;
	int is32bits = 0;

	loader = fopen(argv[1], "rb");
	out = fopen(argv[2], "wb");
	char sig[4] = {0x72, 'h', 'u', 0x4e};
	fwrite(sig, 1, 4, out);

	fseek(loader, 0, SEEK_END);
	loader_size = ftell(loader);
	Elf32_Ehdr *ehdr32 = malloc(loader_size);
	fseek(loader, 0, SEEK_SET);
	fread(ehdr32, 1, loader_size, loader);
	entry = ehdr32->e_entry;
	fwrite(&entry, 1, 4, out);
	is32bits = 1;
	int count = 0;

	Elf32_Phdr *phdr32 = (Elf32_Phdr *)(ehdr32->e_phoff + (void *)ehdr32);
	for (int i = 1; i < ehdr32->e_phnum; i++)
	{
		if (phdr32[i].p_type == PT_LOAD)
		{
			count++;
		}
	}
	fwrite(&count, 1, 2, out);

	for (int i = 1; i < ehdr32->e_phnum; i++)
	{
		if (phdr32[i].p_type == PT_LOAD)
		{
			fwrite(&phdr32[i].p_paddr, 1, sizeof(phdr32[i].p_paddr), out);
			fwrite(&phdr32[i].p_filesz, 1, sizeof(phdr32[i].p_filesz), out);
		}
	}
	int location = ftell(out);
	char space = 0;
	for (int i = 0; i < 512 - (location % 512); i++)
	{
		fwrite(&space, 1, 1, out);
	}
	for (int i = 1; i < ehdr32->e_phnum; i++)
	{
		void *data_or_code = phdr32[i].p_offset + (void *)ehdr32;
		fwrite(data_or_code, 1, phdr32[i].p_filesz, out);
		int location = ftell(out);
		char space = 0;
		for (int i = 0; i < 512 - (location % 512); i++)
		{
			fwrite(&space, 1, 1, out);
		}
	}

	fclose(loader);
	fclose(out);
	return 0;
}
