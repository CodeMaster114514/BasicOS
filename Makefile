MAKEFILE_PATH :=$(abspath $(lastword $(MAKEFILE_LIST)))
PWD :=$(subst /Makefile,,$(MAKEFILE_PATH))
ARCH= x86_64
NASM_FLAGS = -I./arch/include/x86-and-x86_64
target_files = Build/mbr.bin Build/CoreLoader.bin

all:$(target_files)
	dd if=/dev/zero of=a.img bs=512 count=131040
	dd if=./Build/mbr.bin of=a.img bs=512 conv=notrunc
	dd if=./Build/CoreLoader.bin of=a.img bs=512 conv=notrunc seek=1

Build/mbr.bin: arch/x86_64/mbr.asm
	nasm arch/x86_64/mbr.asm -o Build/mbr.bin $(NASM_FLAGS)

Build/CoreLoader.bin: arch/x86_64/CoreLoader.asm
	nasm arch/x86_64/CoreLoader.asm -o Build/CoreLoader.bin $(NASM_FLAGS)

clean:
	rm $(target_files)