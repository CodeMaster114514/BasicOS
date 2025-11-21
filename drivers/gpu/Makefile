include configure.mk

OBJECTS = $(OUT_DIR)/vga.o $(OUT_DIR)/Bits32/vga.o 

vga: $(OBJECTS)

$(OUT_DIR)/vga.o: vga/vga.c
	$(CC) $(CC_FLAGS) $^ -o $@

$(OUT_DIR)/Bits32/vga.o: vga/vga.c
	$(CC) $(CC_FLAGS) -m32 $^ -o $@

clean:
	rm -rf $(OBJECTS)
	
