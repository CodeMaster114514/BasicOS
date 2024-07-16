include configure.mk

OBJECTS = $(OUT_DIR)/main.o $(OUT_DIR)/kmemory.o  $(OUT_DIR)/shell.o $(OUT_DIR)/video.o

all: $(OBJECTS)

$(OUT_DIR)/main.o: main.c
	$(CC) $(CC_FLAGS) main.c -o $(OUT_DIR)/main.o -Wno-builtin-declaration-mismatch

$(OUT_DIR)/kmemory.o: memory.c
	$(CC) $(CC_FLAGS) memory.c -o $(OUT_DIR)/kmemory.o

$(OUT_DIR)/shell.o: shell.c
	$(CC) $(CC_FLAGS) shell.c -o $(OUT_DIR)/shell.o -Wno-builtin-declaration-mismatch

$(OUT_DIR)/video.o: video.c
	$(CC) $(CC_FLAGS) video.c -o $(OUT_DIR)/video.o -Wno-builtin-declaration-mismatch
