/*
 * ファイル操作の実装
 * 複数ファイルの処理と圧縮ファイルの保存/読み込み
 */

#include "huffman_compression.h"

#ifdef _WIN32
#include <direct.h>
#define mkdir(path, mode) _mkdir(path)
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

long read_files(char** file_paths, int num_files, unsigned char** combined_data, FileBoundary* boundaries) {
    long total_size = 0;
    
    // 各ファイルのサイズを計算
    for (int i = 0; i < num_files; i++) {
        FILE* file = fopen(file_paths[i], "rb");
        if (!file) {
            printf("エラー: ファイルを開けません: %s\n", file_paths[i]);
            return 0;
        }
        
        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        fclose(file);
        
        // ファイル境界情報を設定
        const char* filename = strrchr(file_paths[i], '/');
        if (!filename) filename = strrchr(file_paths[i], '\\');
        if (!filename) filename = file_paths[i];
        else filename++; // スラッシュをスキップ
        
        strncpy(boundaries[i].filename, filename, MAX_FILENAME_LENGTH - 1);
        boundaries[i].filename[MAX_FILENAME_LENGTH - 1] = '\0';
        boundaries[i].start_pos = total_size;
        boundaries[i].size = file_size;
        
        total_size += file_size;
    }
    
    if (total_size == 0) {
        *combined_data = NULL;
        return 0;
    }
    
    // メモリ確保
    *combined_data = (unsigned char*)malloc(total_size);
    if (!*combined_data) {
        printf("エラー: メモリ確保に失敗しました\n");
        return 0;
    }
    
    // ファイルを読み込み、結合
    long current_pos = 0;
    for (int i = 0; i < num_files; i++) {
        FILE* file = fopen(file_paths[i], "rb");
        if (!file) {
            free(*combined_data);
            *combined_data = NULL;
            return 0;
        }
        
        long bytes_read = fread(*combined_data + current_pos, 1, boundaries[i].size, file);
        fclose(file);
        
        if (bytes_read != boundaries[i].size) {
            printf("警告: ファイル読み込みサイズが異なります: %s\n", file_paths[i]);
        }
        
        current_pos += bytes_read;
    }
    
    return total_size;
}

void serialize_tree(HuffmanNode* root, unsigned char** buffer, int* pos) {
    if (!root) {
        (*buffer)[(*pos)++] = 0; // NULL ノードのマーカー
        return;
    }
    
    if (root->is_leaf) {
        (*buffer)[(*pos)++] = 1; // 葉ノードのマーカー
        (*buffer)[(*pos)++] = root->character;
    } else {
        (*buffer)[(*pos)++] = 2; // 内部ノードのマーカー
        serialize_tree(root->left, buffer, pos);
        serialize_tree(root->right, buffer, pos);
    }
}

HuffmanNode* deserialize_tree(unsigned char* buffer, int* pos) {
    unsigned char marker = buffer[(*pos)++];
    
    if (marker == 0) {
        return NULL;
    } else if (marker == 1) {
        // 葉ノード
        unsigned char character = buffer[(*pos)++];
        return create_node(character, 0, true);
    } else if (marker == 2) {
        // 内部ノード
        HuffmanNode* node = create_node(0, 0, false);
        if (node) {
            node->left = deserialize_tree(buffer, pos);
            node->right = deserialize_tree(buffer, pos);
        }
        return node;
    }
    
    return NULL;
}

void write_compressed_file(const char* output_path, unsigned char* encoded_data, long encoded_size, 
                          HuffmanNode* tree, int padding, FileBoundary* boundaries, int num_files) {
    FILE* file = fopen(output_path, "wb");
    if (!file) {
        printf("エラー: 出力ファイルを作成できません: %s\n", output_path);
        return;
    }
    
    // ヘッダー情報を準備
    CompressionHeader header;
    header.num_files = num_files;
    header.padding = padding;
    header.data_size = encoded_size;
    
    // ファイル境界情報をコピー
    for (int i = 0; i < num_files; i++) {
        header.file_boundaries[i] = boundaries[i];
    }
    
    // ツリーをシリアライズ
    unsigned char tree_buffer[MAX_TREE_SIZE * 3]; // 余裕を持ったサイズ
    int tree_pos = 0;
    serialize_tree(tree, &tree_buffer, &tree_pos);
    header.tree_size = tree_pos;
    
    // ヘッダーサイズを書き込み
    int header_size = sizeof(CompressionHeader);
    fwrite(&header_size, sizeof(int), 1, file);
    
    // ヘッダーを書き込み
    fwrite(&header, sizeof(CompressionHeader), 1, file);
    
    // ツリーデータを書き込み
    fwrite(tree_buffer, 1, tree_pos, file);
    
    // 圧縮データを書き込み
    fwrite(encoded_data, 1, encoded_size, file);
    
    fclose(file);
}

int read_compressed_file(const char* file_path, unsigned char** encoded_data, HuffmanNode** tree, 
                        int* padding, FileBoundary* boundaries) {
    FILE* file = fopen(file_path, "rb");
    if (!file) {
        printf("エラー: 圧縮ファイルを開けません: %s\n", file_path);
        return 0;
    }
    
    // ヘッダーサイズを読み込み
    int header_size;
    if (fread(&header_size, sizeof(int), 1, file) != 1) {
        printf("エラー: ヘッダーサイズの読み込みに失敗しました\n");
        fclose(file);
        return 0;
    }
    
    // ヘッダーを読み込み
    CompressionHeader header;
    if (fread(&header, sizeof(CompressionHeader), 1, file) != 1) {
        printf("エラー: ヘッダーの読み込みに失敗しました\n");
        fclose(file);
        return 0;
    }
    
    // ファイル境界情報をコピー
    for (int i = 0; i < header.num_files; i++) {
        boundaries[i] = header.file_boundaries[i];
    }
    
    *padding = header.padding;
    
    // ツリーデータを読み込み
    unsigned char tree_buffer[MAX_TREE_SIZE * 3];
    if (fread(tree_buffer, 1, header.tree_size, file) != header.tree_size) {
        printf("エラー: ツリーデータの読み込みに失敗しました\n");
        fclose(file);
        return 0;
    }
    
    // ツリーをデシリアライズ
    int tree_pos = 0;
    *tree = deserialize_tree(tree_buffer, &tree_pos);
    if (!*tree) {
        printf("エラー: ツリーのデシリアライズに失敗しました\n");
        fclose(file);
        return 0;
    }
    
    // 圧縮データを読み込み
    *encoded_data = (unsigned char*)malloc(header.data_size);
    if (!*encoded_data) {
        printf("エラー: データ読み込み用メモリの確保に失敗しました\n");
        destroy_tree(*tree);
        fclose(file);
        return 0;
    }
    
    if (fread(*encoded_data, 1, header.data_size, file) != header.data_size) {
        printf("エラー: 圧縮データの読み込みに失敗しました\n");
        free(*encoded_data);
        destroy_tree(*tree);
        fclose(file);
        return 0;
    }
    
    fclose(file);
    return header.num_files;
}

void extract_files(unsigned char* decoded_data, FileBoundary* boundaries, int num_files, const char* output_dir) {
    // 出力ディレクトリを作成
    if (strcmp(output_dir, ".") != 0) {
        mkdir(output_dir, 0755);
    }
    
    for (int i = 0; i < num_files; i++) {
        // 出力パスを構築
        char output_path[MAX_FILENAME_LENGTH * 2];
        if (strcmp(output_dir, ".") == 0) {
            strcpy(output_path, boundaries[i].filename);
        } else {
            snprintf(output_path, sizeof(output_path), "%s/%s", output_dir, boundaries[i].filename);
            // Windows の場合、スラッシュをバックスラッシュに変換
            #ifdef _WIN32
            for (char* p = output_path; *p; p++) {
                if (*p == '/') *p = '\\';
            }
            #endif
        }
        
        // ファイルを作成
        FILE* output_file = fopen(output_path, "wb");
        if (!output_file) {
            printf("警告: ファイルを作成できません: %s\n", output_path);
            continue;
        }
        
        // データを書き込み
        unsigned char* file_data = decoded_data + boundaries[i].start_pos;
        long bytes_written = fwrite(file_data, 1, boundaries[i].size, output_file);
        fclose(output_file);
        
        if (bytes_written == boundaries[i].size) {
            printf("展開完了: %s\n", output_path);
        } else {
            printf("警告: ファイル書き込みが不完全です: %s\n", output_path);
        }
    }
}

char* generate_compressed_filename(const char* first_file) {
    if (!first_file) return NULL;
    
    // ファイル名からディレクトリとベース名を抽出
    const char* base_name = strrchr(first_file, '/');
    if (!base_name) base_name = strrchr(first_file, '\\');
    if (!base_name) base_name = first_file;
    else base_name++; // スラッシュをスキップ
    
    // 拡張子を除去
    char* temp_name = (char*)malloc(strlen(base_name) + 1);
    strcpy(temp_name, base_name);
    
    char* dot = strrchr(temp_name, '.');
    if (dot) *dot = '\0';
    
    // .cmp拡張子を追加
    char* compressed_name = (char*)malloc(strlen(temp_name) + 5); // .cmp + '\0'
    sprintf(compressed_name, "%s.cmp", temp_name);
    
    free(temp_name);
    return compressed_name;
}
