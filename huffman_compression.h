#ifndef HUFFMAN_COMPRESSION_H
#define HUFFMAN_COMPRESSION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

// 定数定義
#define MAX_FILENAME_LENGTH 256
#define MAX_FILES 100
#define MAX_TREE_SIZE 512
#define BYTE_SIZE 8

// ハフマン木のノード構造体
typedef struct HuffmanNode {
    unsigned char character;
    int frequency;
    struct HuffmanNode* left;
    struct HuffmanNode* right;
    bool is_leaf;
} HuffmanNode;

// 優先度キュー（最小ヒープ）
typedef struct {
    HuffmanNode** nodes;
    int size;
    int capacity;
} MinHeap;

// ファイル境界情報
typedef struct {
    char filename[MAX_FILENAME_LENGTH];
    long start_pos;
    long size;
} FileBoundary;

// 圧縮ヘッダー情報
typedef struct {
    int num_files;
    int padding;
    int tree_size;
    long data_size;
    FileBoundary file_boundaries[MAX_FILES];
} CompressionHeader;

// ハフマン符号テーブル
typedef struct {
    char code[256];  // 最大255ビットの符号
    int length;
} HuffmanCode;

// 関数プロトタイプ宣言

// ヒープ操作
MinHeap* create_min_heap(int capacity);
void destroy_min_heap(MinHeap* heap);
void heap_insert(MinHeap* heap, HuffmanNode* node);
HuffmanNode* heap_extract_min(MinHeap* heap);
void heapify_down(MinHeap* heap, int index);

// ハフマン木操作
HuffmanNode* create_node(unsigned char character, int frequency, bool is_leaf);
void destroy_tree(HuffmanNode* root);
HuffmanNode* build_huffman_tree(int* frequencies);
void generate_codes(HuffmanNode* root, HuffmanCode* codes, char* current_code, int depth);

// ファイル操作
long read_files(char** file_paths, int num_files, unsigned char** combined_data, FileBoundary* boundaries);
void write_compressed_file(const char* output_path, unsigned char* encoded_data, long encoded_size, 
                          HuffmanNode* tree, int padding, FileBoundary* boundaries, int num_files);
int read_compressed_file(const char* file_path, unsigned char** encoded_data, HuffmanNode** tree, 
                        int* padding, FileBoundary* boundaries);
void extract_files(unsigned char* decoded_data, FileBoundary* boundaries, int num_files, const char* output_dir);

// 符号化・復号化
long encode_data(unsigned char* data, long data_size, HuffmanNode* tree, unsigned char** encoded_data, int* padding);
long decode_data(unsigned char* encoded_data, long encoded_size, HuffmanNode* tree, int padding, unsigned char** decoded_data);

// ユーティリティ関数
void serialize_tree(HuffmanNode* root, unsigned char** buffer, int* pos);
HuffmanNode* deserialize_tree(unsigned char* buffer, int* pos);
void calculate_frequencies(unsigned char* data, long size, int* frequencies);
char* generate_compressed_filename(const char* first_file);
void show_usage(void);
void print_statistics(long original_size, long compressed_size, double time_taken);

// メイン関数
int compress_files(char** file_paths, int num_files);
int decompress_file(const char* compressed_path, const char* output_dir);

#endif // HUFFMAN_COMPRESSION_H
