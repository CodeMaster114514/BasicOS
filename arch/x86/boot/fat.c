#include "x86/fat.h"
#include "btype.h"
#include "fs/include/fat.h"
#include "fs/include/mbr.h"
#include "x86/video.h"
#include "x86/disk.h"

UINT32 GetFatPartition()
{
	MBR *mbr = (void *)0x7c00;
	read_boot_disk(0, 1, mbr);
	for (int i = 0; i < 4; ++i)
	{
		if (mbr->partition[i].BootIndicator == 0x80)
			switch (mbr->partition[i].OSType)
			{
			case 0x06:
			case 0x0C:
			case 0x0E:
				return mbr->partition[i].StartLBA;
			default:
				break;
			}
	}
	return 0;
}

UINT8 GetFileSystemType(UINT32 LBA, void *cache)
{
	FAT_BSB *BSB = cache;
	read_boot_disk(LBA, 1, cache);
	if ((BSB->SecCnt16 != 0 && BSB->SecCnt16 < 4085) || (BSB->SecCnt32 != 0 && BSB->SecCnt32 < 4085))
	{
		return FAT12;
	}
	else if ((BSB->SecCnt16 != 0 && BSB->SecCnt16 < 65565) || (BSB->SecCnt32 != 0 && BSB->SecCnt32 < 65565))
	{
		return FAT16;
	}
	else
	{
		return FAT32;
	}
	return 0;
}

UINT32 getStringLenth(const char *path)
{
	UINT32 i = 0;
	while (path[i] != '\0')
		++i;
	return i;
}

bool CheckString(const char *path, const char *target)
{
	UINT32 lenth = getStringLenth(path);
	UINT32 target_p = 0;
	for (UINT32 i = 0; i < lenth; ++i)
	{
		if (path[i] == '.')
			target_p = 8;
		else if ((target[target_p] | 0x20) == path[i])
		{
			++target_p;
		}
		else
		{
			return false;
		}
	}
	return true;
}

int getClusterListCount(UINT32 FAT_Table_LBA, UINT32 ClusterCode, void *cache)
{
	UINT32 *FAT_Table = cache;
	read_boot_disk(FAT_Table_LBA + ClusterCode / (0x200 / 4), 1, cache);
	UINT32 j = ClusterCode % (0x200 / 4);
	int count = 1;
	while (1)
	{
		if (FAT_Table[j] == 0xfffffff6 | FAT_Table[j] == 0xfffffff7)
		{
			count = -1;
			break;
		}
		if (FAT_Table[j] >= 0x0ffffff8)
			break;
		if (ClusterCode / (0x200 / 4) != FAT_Table[j] / (0x200 / 4))
		{
			ClusterCode = FAT_Table[j];
			read_boot_disk(FAT_Table_LBA + ClusterCode / (0x200 / 4), 1, cache);
		}
		j = FAT_Table[j] % (0x200 / 4);
		++count;
	}
	return count;
}

void getClusterList(UINT32 FAT_Table_LBA, UINT32 ClusterCode, UINT32 *ClusterCodeList, void *cache)
{
	UINT32 *FAT_Table = cache;
	ClusterCodeList[0] = ClusterCode;
	read_boot_disk(FAT_Table_LBA + ClusterCode / (0x200 / 4), 1, cache);
	int j = ClusterCode % (0x200 / 4);
	int i = 1;
	while (1)
	{
		if (FAT_Table[j] == 0xfffffff6 || FAT_Table[j] == 0xfffffff7)
		{
			ClusterCodeList[0] = 0;
			break;
		}
		if (FAT_Table[j] >= 0x0ffffff8)
		{
			break;
		}
		ClusterCodeList[i] = FAT_Table[j];
		if (ClusterCode / (0x200 / 4) != FAT_Table[j] / (0x200 / 4))
		{
			ClusterCode = FAT_Table[j];
			read_boot_disk(FAT_Table_LBA + ClusterCode / (0x200 / 4), 1, cache);
		}
		j = FAT_Table[j] % (0x200 / 4);
		++i;
	}
}

void readClusterList(UINT32 *ClusterCodeList, UINT32 DataLBA, int count, UINT32 SecPerClu, void *out)
{
	for (int i = 0; i < count; ++i)
	{
		read_boot_disk((ClusterCodeList[i] - 2) * SecPerClu + DataLBA, SecPerClu, out + 512 * i * SecPerClu);
	}
}
bool readByCluster(UINT32 DataLBA, UINT32 FAT_Table_LBA, UINT32 ClusterCode, UINT32 SecPerClu, void *out)
{
	int count = getClusterListCount(FAT_Table_LBA, ClusterCode, out);
	if (count == -1)
	{
		puts("Error at getClusterListCount.\n");
		return false;
	}
	UINT32 ClusterCodeList[count];
	getClusterList(FAT_Table_LBA, ClusterCode, ClusterCodeList, out);
	if (!ClusterCodeList[0])
	{
		puts("this file was broken.\n");
		return false;
	}
	readClusterList(ClusterCodeList, DataLBA, count, SecPerClu, out);
	return true;
}

ReadResult ReadFileFAT32(UINT32 LBA, const char *path, void *out)
{
	ReadResult result;

	FAT_BSB *BSB = out;
	exFAT_BSB32 *BSB32 = out + sizeof(FAT_BSB);
	read_boot_disk(LBA, 1, out);

	UINT32 DataLBA = BSB->NumsFATs * BSB32->TableSize + LBA + BSB->RsvdSecCount;
	UINT16 SecPerClu = BSB->SecPerClu;

	FS_info *fs_info = out + 512;
	read_boot_disk(LBA + BSB32->FAT_info, 1, fs_info);
	if (fs_info->Signature0 != 0x41615252 || fs_info->Signature1 != 0x61417272)
	{
		puts("Error at read FS_info.\n");
		result.status = false;
		return result;
	}

	UINT32 FAT_Table_LBA = LBA + BSB->RsvdSecCount;
	UINT32 *FAT_Table = out + 512 * 1;
	read_boot_disk(FAT_Table_LBA, 1, FAT_Table);

	UINT32 RootDirLBA = FAT_Table_LBA + BSB32->TableSize * 2;
	FAT32_ENTEY *entrys = out + 512 * 3;
	UINT32 DirSize = getClusterListCount(FAT_Table_LBA, BSB32->RootCluster, entrys);
	if (DirSize == -1)
	{
		puts("this Directory was broken.");
		result.status = false;
		return result;
	}
	UINT32 ClusterCodeList[DirSize];
	getClusterList(FAT_Table_LBA, BSB32->RootCluster, ClusterCodeList, entrys);
	for (int i = 0; i < DirSize; ++i)
	{
		read_boot_disk((ClusterCodeList[i] - 2) * SecPerClu + RootDirLBA, SecPerClu, out + 512 * 3);
		for (int j = 0; j < 512 / sizeof(FAT32_ENTEY); ++j)
		{
			if (entrys[j].name[0] == 0x00 || entrys[j].name[0] == 0xE5) // 此目录为无效目录
				continue;
			;
			if (entrys[j].flags == 0x0f) // 此目录为长文件名
				continue;
			else if (CheckString(path, entrys[j].name)) // 此目录为短文件名
			{
				result.length = entrys[j].Length;
				UINT32 ClusterCode = entrys[j].ClusterCodeHigh << 16 | entrys[j].ClusterCodeLow;
				result.status = readByCluster(DataLBA, FAT_Table_LBA, ClusterCode, SecPerClu, out);
				return result;
			}
		}
	}
	puts("We can't find the file.\n");
	result.status = false;
	return result;
}

ReadResult readFile(const char *path, void *out)
{
	UINT32 LBA = GetFatPartition();
	ReadResult result;
	if (!LBA)
	{
		puts("The disk haven't a fat partition.\n");
		result.status = false;
		return result;
	}
	UINT8 FileSystemType = GetFileSystemType(LBA, out);
	if (!FileSystemType || FileSystemType == FAT12)
	{
		puts("We can't find a fat file system which use LBA.\n");
		result.status = false;
		return result;
	}
	switch (FileSystemType)
	{
	case FAT16:
		break;
	case FAT32:
		result = ReadFileFAT32(LBA, path, out);
		break;
	}
	return result;
}
