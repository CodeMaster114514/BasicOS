MAKEFILE_PATH :=$(abspath $(lastword $(MAKEFILE_LIST)))
PWD :=$(subst /Makefile,,$(MAKEFILE_PATH))
ARCH= x86_64
NASM_FLAGS = -I./arch/include/x86-and-x86_64 -I./arch/x86_64/boot
CC_FLAG = -I./arch/include
target_files = Build/mbr.bin Build/CoreLoader.bin

all:$(target_files)
	dd if=./Build/mbr.bin of=a.img bs=512 conv=notrunc
	dd if=./Build/CoreLoader.bin of=a.img bs=512 conv=notrunc seek=2048

Build/mbr.bin: arch/x86_64/boot/mbr.asm
	nasm arch/x86_64/boot/mbr.asm -o Build/mbr.bin $(NASM_FLAGS)

Build/CoreLoader.bin: arch/x86_64/boot/CoreLoader.asm
	nasm arch/x86_64/boot/CoreLoader.asm -o Build/CoreLoader.bin $(NASM_FLAGS)

clean:
	rm $(target_files)