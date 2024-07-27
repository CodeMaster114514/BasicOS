include configure.mk

NASM_FLAGS = -I$(PWD)/arch/include/x86-and-x86_64

all: $(OUT_DIR)/mbr.bin $(OUT_DIR)/CoreLoader.bin

$(OUT_DIR)/mbr.bin: mbr.asm
	nasm $(NASM_FLAGS) mbr.asm -o $(OUT_DIR)/mbr.bin

$(OUT_DIR)/CoreLoader.bin: CoreLoader.asm CoreLoaderInC.asm
	nasm $(NASM_FLAGS) CoreLoader.asm -o $(OUT_DIR)/CoreLoader.bin

CoreLoaderInC.asm: CoreLoaderInC.c
	$(CC) $(CC_FLAGS) CoreLoaderInC.c -mno-sse2
	objconv -fnasm CoreLoaderInC.o CoreLoaderInC.asm
