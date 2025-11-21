#include "kernel.h"
#include "config.h"
#include "btype.h"
#include "memory.h"
#include "video.h"
#include "shell.h"
#include "interrupt.h"
#if X86_64

#include "x86/sse.h"
#include "x86/cpuid.h"
#include "x86/memory.h"

#endif

int a = 9;
const char *b = "abc";
extern bool set;

extern MemoryConfigure configure;

/**
 * @brief 内核初始化函数
 *
 * 该函数负责初始化系统的各个组件，包括视频输出、内存管理、中断处理等。
 * 在初始化完成后，程序将进入一个无限循环，等待中断发生。
 *
 * @param table 指向系统配置表的指针
 * @return int 函数执行成功返回0，否则返回非0值
 */
int kernel(Table *table)
{
    // 初始化视频输出
    InitVideo(&table->gconfigre);

#if X86_64
    // 如果是X86_64架构，启用SSE指令集
    EnableSSE();
#endif

    // 输出系统信息
    showCPUInfo();
    showCPUBrand();

    puts("LinearAddrSize: %d\nPhysicalAddrSize: %d\n", table->LinearAddrSize, table->PhysicalAddrSize);
    // 初始化内存管理
    InitMemory(table->mmap, table->map_count, table->LinearAddrSize, table->PhysicalAddrSize);
    // 初始化中断处理
    //InitInterrupt();

    // 输出当前运行模式
    puts("We are running in the ia32e mode\n");

    // 输出一个数字
    puts("The number is %d\n", 512);

    puts("The hex is %p\n", 0x10);

    UINT64 count = 0;
    bool check = false;
    while (1)
    {
        UINT32 *test = allocA2MBPage(MAP_TYPE_USER_DATA, MAP_FLAG_WRITE | MAP_FLAG_READ);
        *test = count;
        count++;
        if (count == 400)
            break;
        if (test == NULL)
        {
            puts("There is no free memory space.\n");
            break;
        }
        if (test == 0x40800000)
        {
            check = true;
            set = true;
        }
        if (check)
        {
            puts("data at %d\n", *(UINT32 *)0x40800000);
        }
        puts("page count %d: %p\n", count, test);
    }

    /*MMAP *mmap = configure.mmap;
    while (1)
    {
        puts("Type: %d;Address: %p - %p\n", mmap->type, mmap->PhysicalAddress, mmap->PhysicalAddress + 0x1000 * mmap->NoOfPage);
        if (mmap->isLast)
        {
            break;
        }
        else
        {
            mmap = mmap->next ? mmap->next : &mmap[1];
        }
    }*/
    

    //allocA4KBPage(MAP_TYPE_OSDATA, MAP_FLAG_WRITE | MAP_FLAG_READ);*/

// 进入无限循环，等待中断
end:
    for (;;)
    {
        // 使用汇编指令hlt使CPU进入低功耗状态，直到有中断信号唤醒它
        asm("hlt\n\t");
    }
}
