# 定義編譯器和編譯選項
CC = gcc
CFLAGS = -Wall -Wextra -std=c99

# 目標執行檔名稱
TARGET = hsh

# 源文件和對應的頭文件
SOURCES = main.c hsh.c
HEADERS = hsh.h

# 默認目標
all: $(TARGET)

# 編譯執行檔（直接編譯源文件，不生成中間的 .o 文件）
$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(SOURCES)

# 清理執行檔
clean:
	rm -rf $(TARGET)

# 運行程序
run: $(TARGET)
	./$(TARGET)

# 假目標
.PHONY: all clean run