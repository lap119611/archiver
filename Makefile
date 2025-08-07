# Makefile for Huffman Compression (C Version)

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
TARGET = huffman_compression
SOURCES = huffman_compression.c huffman_tree.c file_handler.c
OBJECTS = $(SOURCES:.c=.o)
HEADERS = huffman_compression.h

# Windows特有の設定
ifeq ($(OS),Windows_NT)
    TARGET := $(TARGET).exe
    RM = del /Q
    MKDIR = mkdir
else
    RM = rm -f
    MKDIR = mkdir -p
endif

# デフォルトターゲット
all: $(TARGET)

# 実行ファイルの作成
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

# オブジェクトファイルの作成
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# テスト用ターゲット
test: $(TARGET)
	@echo "基本テストを実行中..."
	@echo "Hello, World! This is a test file." > test_input.txt
	@echo "Another test file content." > test_input2.txt
	./$(TARGET) compress test_input.txt test_input2.txt
	./$(TARGET) decompress test_input.cmp extracted
	@echo "テスト完了"

# クリーンアップ
clean:
ifeq ($(OS),Windows_NT)
	if exist *.o $(RM) *.o
	if exist $(TARGET) $(RM) $(TARGET)
	if exist *.cmp $(RM) *.cmp
	if exist test_input*.txt $(RM) test_input*.txt
	if exist extracted $(RM) /Q /S extracted
else
	$(RM) $(OBJECTS) $(TARGET) *.cmp test_input*.txt
	$(RM) -rf extracted
endif

# デバッグ版
debug: CFLAGS += -g -DDEBUG
debug: $(TARGET)

# リリース版（最適化強化）
release: CFLAGS += -O3 -DNDEBUG
release: clean $(TARGET)

# ヘルプ
help:
	@echo "利用可能なターゲット:"
	@echo "  all     - プログラムをビルド (デフォルト)"
	@echo "  test    - ビルドしてテストを実行"
	@echo "  clean   - 生成されたファイルを削除"
	@echo "  debug   - デバッグ版をビルド"
	@echo "  release - 最適化版をビルド"
	@echo "  help    - このヘルプを表示"
	@echo ""
	@echo "使用例:"
	@echo "  make"
	@echo "  make test"
	@echo "  make clean"

.PHONY: all test clean debug release help
