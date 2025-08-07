/*
 * ハフマン符号による圧縮・解凍プログラム (C言語版)
 * 
 * 使用法:
 *   圧縮: huffman_compression compress file1.txt [file2.txt ...]
 *   解凍: huffman_compression decompress compressed_file.cmp [output_directory]
 * 
 * 機能:
 * - 複数ファイルの一括圧縮
 * - ハフマン符号による高効率圧縮
 * - 元ファイル構造の保持
 */

#include "huffman_compression.h"

// グローバル変数
static HuffmanCode code_table[256];

int main(int argc, char* argv[]) {
    if (argc < 3) {
        show_usage();
        return 1;
    }
    
    char* command = argv[1];
    
    // 文字列を小文字に変換
    for (int i = 0; command[i]; i++) {
        command[i] = tolower(command[i]);
    }
    
    if (strcmp(command, "compress") == 0) {
        // 圧縮モード
        if (argc < 3) {
            printf("エラー: 圧縮対象ファイルが指定されていません\n");
            show_usage();
            return 1;
        }
        
        return compress_files(&argv[2], argc - 2);
        
    } else if (strcmp(command, "decompress") == 0) {
        // 解凍モード
        if (argc < 3) {
            printf("エラー: 圧縮ファイルが指定されていません\n");
            show_usage();
            return 1;
        }
        
        char* compressed_file = argv[2];
        char* output_dir = (argc > 3) ? argv[3] : ".";
        
        return decompress_file(compressed_file, output_dir);
        
    } else {
        printf("エラー: 不明なコマンド '%s'\n", command);
        show_usage();
        return 1;
    }
}

int compress_files(char** file_paths, int num_files) {
    printf("圧縮開始: %d個のファイル\n", num_files);
    clock_t start_time = clock();
    
    // ファイル存在確認
    for (int i = 0; i < num_files; i++) {
        FILE* file = fopen(file_paths[i], "rb");
        if (!file) {
            printf("エラー: ファイルが見つかりません: %s\n", file_paths[i]);
            return 1;
        }
        fclose(file);
    }
    
    // ファイル読み込み
    printf("ファイル読み込み中...\n");
    unsigned char* combined_data = NULL;
    FileBoundary boundaries[MAX_FILES];
    long original_size = read_files(file_paths, num_files, &combined_data, boundaries);
    
    if (original_size == 0) {
        printf("警告: 圧縮対象のデータが空です\n");
        free(combined_data);
        return 0;
    }
    
    printf("元データサイズ: %ld バイト\n", original_size);
    
    // 頻度計算
    int frequencies[256] = {0};
    calculate_frequencies(combined_data, original_size, frequencies);
    
    // ハフマン木構築
    printf("ハフマン符号化中...\n");
    HuffmanNode* tree = build_huffman_tree(frequencies);
    if (!tree) {
        printf("エラー: ハフマン木の構築に失敗しました\n");
        free(combined_data);
        return 1;
    }
    
    // 符号テーブル生成
    memset(code_table, 0, sizeof(code_table));
    char current_code[256] = {0};
    generate_codes(tree, code_table, current_code, 0);
    
    // データ符号化
    unsigned char* encoded_data = NULL;
    int padding = 0;
    long encoded_size = encode_data(combined_data, original_size, tree, &encoded_data, &padding);
    
    if (encoded_size == 0) {
        printf("エラー: データの符号化に失敗しました\n");
        free(combined_data);
        destroy_tree(tree);
        return 1;
    }
    
    // 圧縮ファイル名生成
    char* output_path = generate_compressed_filename(file_paths[0]);
    
    // 圧縮ファイル書き込み
    printf("圧縮ファイル書き込み中...\n");
    write_compressed_file(output_path, encoded_data, encoded_size, tree, padding, boundaries, num_files);
    
    // 統計情報表示
    FILE* output_file = fopen(output_path, "rb");
    fseek(output_file, 0, SEEK_END);
    long compressed_size = ftell(output_file);
    fclose(output_file);
    
    clock_t end_time = clock();
    double time_taken = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    
    printf("\n圧縮完了!\n");
    printf("出力ファイル: %s\n", output_path);
    print_statistics(original_size, compressed_size, time_taken);
    
    // メモリ解放
    free(combined_data);
    free(encoded_data);
    free(output_path);
    destroy_tree(tree);
    
    return 0;
}

int decompress_file(const char* compressed_path, const char* output_dir) {
    printf("解凍開始: %s\n", compressed_path);
    clock_t start_time = clock();
    
    // 圧縮ファイル存在確認
    FILE* file = fopen(compressed_path, "rb");
    if (!file) {
        printf("エラー: 圧縮ファイルが見つかりません: %s\n", compressed_path);
        return 1;
    }
    fclose(file);
    
    // 圧縮ファイル読み込み
    printf("圧縮ファイル読み込み中...\n");
    unsigned char* encoded_data = NULL;
    HuffmanNode* tree = NULL;
    int padding = 0;
    FileBoundary boundaries[MAX_FILES];
    
    int num_files = read_compressed_file(compressed_path, &encoded_data, &tree, &padding, boundaries);
    if (num_files == 0) {
        printf("エラー: 圧縮ファイルの読み込みに失敗しました\n");
        return 1;
    }
    
    printf("元ファイル数: %d\n", num_files);
    
    // データ復号化
    printf("ハフマン復号化中...\n");
    unsigned char* decoded_data = NULL;
    
    // 圧縮データサイズを計算
    file = fopen(compressed_path, "rb");
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fclose(file);
    
    // ヘッダーサイズを除いたデータサイズを推定
    long encoded_size = file_size - sizeof(CompressionHeader) - 1024; // 概算
    
    long decoded_size = decode_data(encoded_data, encoded_size, tree, padding, &decoded_data);
    if (decoded_size == 0) {
        printf("エラー: データの復号化に失敗しました\n");
        free(encoded_data);
        destroy_tree(tree);
        return 1;
    }
    
    printf("復号化後サイズ: %ld バイト\n", decoded_size);
    
    // ファイル抽出
    printf("ファイル抽出中...\n");
    extract_files(decoded_data, boundaries, num_files, output_dir);
    
    clock_t end_time = clock();
    double time_taken = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    
    printf("\n解凍完了!\n");
    printf("出力ディレクトリ: %s\n", output_dir);
    printf("処理時間: %.2f秒\n", time_taken);
    
    // メモリ解放
    free(encoded_data);
    free(decoded_data);
    destroy_tree(tree);
    
    return 0;
}

void show_usage() {
    printf("ハフマン符号圧縮・解凍プログラム (C言語版)\n");
    printf("\n使用法:\n");
    printf("  圧縮: huffman_compression compress <ファイル1> [ファイル2] ...\n");
    printf("  解凍: huffman_compression decompress <圧縮ファイル.cmp> [出力ディレクトリ]\n");
    printf("\n例:\n");
    printf("  huffman_compression compress document.txt image.jpg\n");
    printf("  huffman_compression decompress document.cmp ./extracted/\n");
    printf("\n注意:\n");
    printf("  - 複数ファイルは一つの.cmpファイルにまとめられます\n");
    printf("  - 圧縮ファイル名は最初のファイル名に.cmpを付加したものになります\n");
    printf("  - 解凍時は元のファイル名で復元されます\n");
}

void print_statistics(long original_size, long compressed_size, double time_taken) {
    printf("圧縮後サイズ: %ld バイト\n", compressed_size);
    if (original_size > 0) {
        double compression_ratio = (1.0 - (double)compressed_size / original_size) * 100.0;
        printf("圧縮率: %.2f%%\n", compression_ratio);
    }
    printf("処理時間: %.2f秒\n", time_taken);
}
