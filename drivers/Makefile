include configure.mk

DRIVERS = 

ifeq ($(GPU_VGA), 1)

DRIVERS += vga

endif

all: $(DRIVERS)

vga:
	make -C gpu -I$(ROOT) OUT_DIR=$(OUT_DIR) ROOT=$(ROOT) vga