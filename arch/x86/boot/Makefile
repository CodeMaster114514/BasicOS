include configure.mk

OBJECTS = $(OUT_DIR)/Boot/loader.o $(OUT_DIR)/Bits32/vga.o $(OUT_DIR)/Boot/video.o $(OUT_DIR)/Boot/memory.o $(OUT_DIR)/Boot/disk.o $(OUT_DIR)/Boot/fat.o $(OUT_DIR)/Boot/interrupt.o

all: $(OUT_DIR)/Boot/mbr.bin $(OUT_DIR)/Boot/loader.bin

$(OUT_DIR)/Boot/mbr.bin: mbr.asm
	$(AS) $(AS_FLAGS) $< -o $@

$(OUT_DIR)/Boot/loader.o: loader.c
	$(CC) -m32 $(CC_FLAGS) $< -o $@

$(OUT_DIR)/Boot/video.o: video.c
	$(CC) -m32 $(CC_FLAGS) $^ -o $@

$(OUT_DIR)/Boot/memory.o: memory.c
	$(CC) -m32 $(CC_FLAGS) $^ -o $@

$(OUT_DIR)/Boot/disk.o: disk.c
	$(CC) -m32 $(CC_FLAGS) $^ -o $@

$(OUT_DIR)/Boot/fat.o: fat.c
	$(CC) -m32 $(CC_FLAGS) $^ -o $@

$(OUT_DIR)/Boot/interrupt.o: interrupt.c
	$(CC) -m32 $(CC_FLAGS) -mgeneral-regs-only $^ -o $@


$(OUT_DIR)/Boot/loader: $(OBJECTS)
	$(LD) -O0 -m elf_i386 --no-relax -nostdlib -Ttext 0x50000  $^ -o $@

#$(OUT_DIR)/Boot/loader-text.bin: $(OUT_DIR)/Boot/loader
#	objcopy --dump-section=.text=$@ $^

#$(OUT_DIR)/Boot/loader-data.bin: $(OUT_DIR)/Boot/loader
#	objcopy --dump-section=.data=$@ $^

#$(OUT_DIR)/Boot/loader-rodata.bin: $(OUT_DIR)/Boot/loader
#	objcopy --dump-section=.rodata=$@ $^

$(OUT_DIR)/Boot/loader.bin: $(OUT_DIR)/Boot/loader #$(OUT_DIR)/Boot/loader-text.bin $(OUT_DIR)/Boot/loader-data.bin $(OUT_DIR)/Boot/loader-rodata.bin
	$(OUT_DIR)/LoaderTool $^ $@
#	cat $^ > $(OUT_DIR)/Boot/loader.bin
#$(OUT_DIR)/LoaderHeader.bin: $(OUT_DIR)/Boot/loader $(OUT_DIR)/Boot/loader.bin
clean:
	rm -rf $(OBJECTS)
