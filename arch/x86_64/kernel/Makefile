include configure.mk

OBJECTS = $(OUT_DIR)/memory.o $(OUT_DIR)/sse.o

all: $(OBJECTS)

$(OUT_DIR)/memory.o: memory.c
	$(CC) $(CC_FLAGS) memory.c -o $(OUT_DIR)/memroy.o

$(OUT_DIR)/sse.o: sse.c
	$(CC) $(CC_FLAGS) sse.c -o $(OUT_DIR)/sse.o
