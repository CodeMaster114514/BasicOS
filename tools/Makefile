include configure.mk
# 编译
all: $(OUT_DIR)/LoaderTool

$(OUT_DIR)/LoaderTool: $(OUT_DIR)/tools/LoaderTool.o
	$(CC) -g $^ -o $@

$(OUT_DIR)/tools/LoaderTool.o: LoaderTool.c
	$(CC) -g -c $< -o $@

clean:
	rm -rf $(OUT_DIR)/LoaderTool $(OUT_DIR)/tools/LoaderTool.o