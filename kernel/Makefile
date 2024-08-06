include configure.mk

OBJECTS = $(OUT_DIR)/main.o $(OUT_DIR)/kmemory.o  $(OUT_DIR)/shell.o $(OUT_DIR)/video.o $(OUT_DIR)/mmaps.o

all: $(OBJECTS)

$(OUT_DIR)/main.o: main.c
	$(CC) $(CC_FLAGS) main.c -o $(OUT_DIR)/main.o

$(OUT_DIR)/kmemory.o: memory.c
	$(CC) $(CC_FLAGS) memory.c -o $(OUT_DIR)/kmemory.o

$(OUT_DIR)/mmaps.o: mmaps.c
	$(CC) $(CC_FLAGS) mmaps.c -o $(OUT_DIR)/mmaps.o

$(OUT_DIR)/shell.o: shell.c
	$(CC) $(CC_FLAGS) shell.c -o $(OUT_DIR)/shell.o

$(OUT_DIR)/video.o: video.c
	$(CC) $(CC_FLAGS) video.c -o $(OUT_DIR)/video.o
