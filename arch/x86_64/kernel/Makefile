include configure.mk

OBJECTS = $(OUT_DIR)/memory.o $(OUT_DIR)/sse.o $(OUT_DIR)/interrupt.o

all: $(OBJECTS)

$(OUT_DIR)/memory.o: memory.c
	$(CC) $(CC_FLAGS) -nostdinc memory.c -o $(OUT_DIR)/memory.o

$(OUT_DIR)/sse.o: sse.c
	$(CC) $(CC_FLAGS) -nostdinc sse.c -o $(OUT_DIR)/sse.o

$(OUT_DIR)/interrupt.o: interrupt.c
	$(CC) $(CC_FLAGS) -nostdinc -mgeneral-regs-only interrupt.c -o $(OUT_DIR)/interrupt.o
