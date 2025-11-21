include configure.mk

ifeq ($(ARCH), x86)

all:
	make -C x86 -I$(ROOT) OUT_DIR=$(OUT_DIR) ROOT=$(ROOT) -e

clean:
	make -C x86 -I$(ROOT) OUT_DIR=$(OUT_DIR) ROOT=$(ROOT) clean -e
endif
