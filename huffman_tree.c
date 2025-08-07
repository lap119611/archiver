/*
 * ハフマン木の実装
 * ハフマン符号化・復号化のためのツリー操作
 */

#include "huffman_compression.h"
#include <ctype.h>

// ヒープ操作の実装

MinHeap* create_min_heap(int capacity) {
    MinHeap* heap = (MinHeap*)malloc(sizeof(MinHeap));
    if (!heap) return NULL;
    
    heap->nodes = (HuffmanNode**)malloc(capacity * sizeof(HuffmanNode*));
    if (!heap->nodes) {
        free(heap);
        return NULL;
    }
    
    heap->size = 0;
    heap->capacity = capacity;
    return heap;
}

void destroy_min_heap(MinHeap* heap) {
    if (heap) {
        free(heap->nodes);
        free(heap);
    }
}

void swap_nodes(HuffmanNode** a, HuffmanNode** b) {
    HuffmanNode* temp = *a;
    *a = *b;
    *b = temp;
}

void heapify_up(MinHeap* heap, int index) {
    if (index == 0) return;
    
    int parent = (index - 1) / 2;
    if (heap->nodes[parent]->frequency > heap->nodes[index]->frequency) {
        swap_nodes(&heap->nodes[parent], &heap->nodes[index]);
        heapify_up(heap, parent);
    }
}

void heapify_down(MinHeap* heap, int index) {
    int left = 2 * index + 1;
    int right = 2 * index + 2;
    int smallest = index;
    
    if (left < heap->size && 
        heap->nodes[left]->frequency < heap->nodes[smallest]->frequency) {
        smallest = left;
    }
    
    if (right < heap->size && 
        heap->nodes[right]->frequency < heap->nodes[smallest]->frequency) {
        smallest = right;
    }
    
    if (smallest != index) {
        swap_nodes(&heap->nodes[index], &heap->nodes[smallest]);
        heapify_down(heap, smallest);
    }
}

void heap_insert(MinHeap* heap, HuffmanNode* node) {
    if (heap->size >= heap->capacity) return;
    
    heap->nodes[heap->size] = node;
    heapify_up(heap, heap->size);
    heap->size++;
}

HuffmanNode* heap_extract_min(MinHeap* heap) {
    if (heap->size <= 0) return NULL;
    
    HuffmanNode* min_node = heap->nodes[0];
    heap->nodes[0] = heap->nodes[heap->size - 1];
    heap->size--;
    
    if (heap->size > 0) {
        heapify_down(heap, 0);
    }
    
    return min_node;
}

// ハフマンノード操作

HuffmanNode* create_node(unsigned char character, int frequency, bool is_leaf) {
    HuffmanNode* node = (HuffmanNode*)malloc(sizeof(HuffmanNode));
    if (!node) return NULL;
    
    node->character = character;
    node->frequency = frequency;
    node->left = NULL;
    node->right = NULL;
    node->is_leaf = is_leaf;
    
    return node;
}

void destroy_tree(HuffmanNode* root) {
    if (!root) return;
    
    destroy_tree(root->left);
    destroy_tree(root->right);
    free(root);
}

HuffmanNode* build_huffman_tree(int* frequencies) {
    // 有効な文字をカウント
    int char_count = 0;
    for (int i = 0; i < 256; i++) {
        if (frequencies[i] > 0) {
            char_count++;
        }
    }
    
    if (char_count == 0) return NULL;
    
    // 特殊ケース：1文字のみの場合
    if (char_count == 1) {
        for (int i = 0; i < 256; i++) {
            if (frequencies[i] > 0) {
                HuffmanNode* root = create_node(0, frequencies[i], false);
                root->left = create_node(i, frequencies[i], true);
                return root;
            }
        }
    }
    
    // ヒープを作成
    MinHeap* heap = create_min_heap(char_count);
    if (!heap) return NULL;
    
    // 各文字のノードをヒープに追加
    for (int i = 0; i < 256; i++) {
        if (frequencies[i] > 0) {
            HuffmanNode* node = create_node(i, frequencies[i], true);
            if (node) {
                heap_insert(heap, node);
            }
        }
    }
    
    // ハフマン木を構築
    while (heap->size > 1) {
        HuffmanNode* left = heap_extract_min(heap);
        HuffmanNode* right = heap_extract_min(heap);
        
        if (!left || !right) break;
        
        HuffmanNode* merged = create_node(0, left->frequency + right->frequency, false);
        if (!merged) break;
        
        merged->left = left;
        merged->right = right;
        
        heap_insert(heap, merged);
    }
    
    HuffmanNode* root = (heap->size > 0) ? heap->nodes[0] : NULL;
    destroy_min_heap(heap);
    
    return root;
}

void generate_codes(HuffmanNode* root, HuffmanCode* codes, char* current_code, int depth) {
    if (!root) return;
    
    if (root->is_leaf) {
        // 特殊ケース：ルートが葉の場合
        if (depth == 0) {
            strcpy(codes[root->character].code, "0");
            codes[root->character].length = 1;
        } else {
            strcpy(codes[root->character].code, current_code);
            codes[root->character].length = depth;
        }
        return;
    }
    
    // 左の子（ビット0）
    if (root->left) {
        current_code[depth] = '0';
        current_code[depth + 1] = '\0';
        generate_codes(root->left, codes, current_code, depth + 1);
    }
    
    // 右の子（ビット1）
    if (root->right) {
        current_code[depth] = '1';
        current_code[depth + 1] = '\0';
        generate_codes(root->right, codes, current_code, depth + 1);
    }
    
    current_code[depth] = '\0';
}

void calculate_frequencies(unsigned char* data, long size, int* frequencies) {
    for (long i = 0; i < size; i++) {
        frequencies[data[i]]++;
    }
}

long encode_data(unsigned char* data, long data_size, HuffmanNode* tree, 
                unsigned char** encoded_data, int* padding) {
    
    extern HuffmanCode code_table[256];
    
    // 必要なビット数を計算
    long total_bits = 0;
    for (long i = 0; i < data_size; i++) {
        total_bits += code_table[data[i]].length;
    }
    
    if (total_bits == 0) return 0;
    
    // パディングを計算
    *padding = (8 - (total_bits % 8)) % 8;
    long total_bits_padded = total_bits + *padding;
    long encoded_bytes = total_bits_padded / 8;
    
    // メモリ確保
    *encoded_data = (unsigned char*)calloc(encoded_bytes, sizeof(unsigned char));
    if (!*encoded_data) return 0;
    
    // ビットバッファ
    int bit_pos = 0;
    unsigned char current_byte = 0;
    long byte_index = 0;
    
    // データを符号化
    for (long i = 0; i < data_size; i++) {
        char* code = code_table[data[i]].code;
        int code_len = code_table[data[i]].length;
        
        for (int j = 0; j < code_len; j++) {
            if (code[j] == '1') {
                current_byte |= (1 << (7 - bit_pos));
            }
            
            bit_pos++;
            if (bit_pos == 8) {
                (*encoded_data)[byte_index++] = current_byte;
                current_byte = 0;
                bit_pos = 0;
            }
        }
    }
    
    // 最後のバイトを処理
    if (bit_pos > 0) {
        (*encoded_data)[byte_index] = current_byte;
    }
    
    return encoded_bytes;
}

long decode_data(unsigned char* encoded_data, long encoded_size, HuffmanNode* tree, 
                int padding, unsigned char** decoded_data) {
    if (!encoded_data || !tree || encoded_size == 0) return 0;
    
    // ビット列を展開
    long total_bits = encoded_size * 8 - padding;
    if (total_bits <= 0) return 0;
    
    // デコード用のバッファを確保（最大可能サイズ）
    long max_decoded_size = total_bits; // 最悪の場合、各ビットが1文字
    *decoded_data = (unsigned char*)malloc(max_decoded_size);
    if (!*decoded_data) return 0;
    
    long decoded_count = 0;
    HuffmanNode* current_node = tree;
    
    // 各ビットを処理
    for (long byte_idx = 0; byte_idx < encoded_size; byte_idx++) {
        unsigned char current_byte = encoded_data[byte_idx];
        
        for (int bit_idx = 7; bit_idx >= 0; bit_idx--) {
            // 処理済みビット数をチェック
            long processed_bits = byte_idx * 8 + (7 - bit_idx);
            if (processed_bits >= total_bits) break;
            
            // 特殊ケース：ルートが葉の場合
            if (current_node->is_leaf) {
                (*decoded_data)[decoded_count++] = current_node->character;
                current_node = tree;
            }
            
            // ビットに応じてツリーを下る
            int bit = (current_byte >> bit_idx) & 1;
            if (bit == 0) {
                current_node = current_node->left;
            } else {
                current_node = current_node->right;
            }
            
            // 葉に到達した場合
            if (current_node && current_node->is_leaf) {
                if (decoded_count < max_decoded_size) {
                    (*decoded_data)[decoded_count++] = current_node->character;
                }
                current_node = tree;
            }
        }
    }
    
    return decoded_count;
}
