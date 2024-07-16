MAKEFILE_PATH :=$(abspath $(lastword $(MAKEFILE_LIST)))
PWD :=$(subst /Makefile,,$(MAKEFILE_PATH))

include configure.mk

NASM_FLAGS = -I./arch/include/x86-and-x86_64 -I./Build
OBJCOPY = objcopy
GCC = gcc
CC_FLAGS = -I$(PWD)/arch/include -I$(PWD)/include -I$(PWD)/drivers/include -I$(PWD) -masm=intel -c -fno-stack-protector
LD_FLAGS = -Ttext 0xffff800000400000 -e kernel --no-pie
OBJCOPY_FLAG = --only-keep-section .text --only-keep-section .data --only-keep-section .rodata
target_files = Build/mbr.bin Build/CoreLoader.bin Build/kernel
OBJECTS = Build/main.o Build/memory.o

ifeq ($(ARCH), x86_64)
endif

all:
	make -C arch -I$(PWD) OUT_DIR=$(PWD)/Build ROOT=$(PWD) CC_FLAGS="$(CC_FLAGS)"
	make -C kernel -I$(PWD) OUT_DIR=$(PWD)/Build ROOT=$(PWD) CC_FLAGS="$(CC_FLAGS)"
	make -C drivers -I$(PWD) OUT_DIR=$(PWD)/Build ROOT=$(PWD) CC_FLAGS="$(CC_FLAGS)"
	$(LD) $(PWD)/Build/*.o $(LD_FLAGS) -o Build/kernel
	dd if=./Build/mbr.bin of=a.img bs=512 conv=notrunc
	dd if=./Build/CoreLoader.bin of=a.img bs=512 conv=notrunc seek=2048

Build/mbr.bin: arch/x86_64/boot/mbr.asm
	nasm arch/x86_64/boot/mbr.asm -o Build/mbr.bin $(NASM_FLAGS)

Build/CoreLoader.bin: arch/x86_64/boot/CoreLoader.asm Build/CoreLoaderInC.asm
	nasm arch/x86_64/boot/CoreLoader.asm -o Build/CoreLoader.bin $(NASM_FLAGS)

Build/CoreLoaderInC.o: arch/x86_64/boot/CoreLoaderInC.c
	$(CC) $(CC_FLAGS) arch/x86_64/boot/CoreLoaderInC.c -o Build/CoreLoaderInC.o

Build/CoreLoaderInC.asm: Build/CoreLoaderInC.o
	objconv -fnasm Build/CoreLoaderInC.o Build/CoreLoaderInC.asm

Build/kernel:$(OBJECTS)
	$(LD) $(OBJECTS) $(LD_FLAGS) -o Build/kernel

Build/main.o:kernel/main.c
	$(CC) ./kernel/main.c $(CC_FLAGS) -o Build/main.o

Build/memory:kernel/memory.c
	$(CC) ./kernel/memory.c $(CC_FLAGS) -o Build/memory.o

clean:
	rm Build/*.o Build/*.bin
