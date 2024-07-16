include configure.mk

all: $(OBJECTS)
	make -C boot -I$(ROOT) OUT_DIR=$(OUT_DIR) ROOT=$(ROOT)
	make -C kernel -I$(ROOT) OUT_DIR=$(OUT_DIR) ROOT=$(ROOT)