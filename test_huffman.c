/*
 * ハフマン圧縮プログラムの簡易テスト
 */

#include "huffman_compression.h"

void create_test_files() {
    // テストファイル1: 英語テキスト
    FILE* f1 = fopen("test_english.txt", "w");
    fprintf(f1, "Hello, World!\n");
    fprintf(f1, "This is a test file for Huffman compression.\n");
    fprintf(f1, "The quick brown fox jumps over the lazy dog.\n");
    fprintf(f1, "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n");
    fprintf(f1, "abcdefghijklmnopqrstuvwxyz\n");
    fprintf(f1, "0123456789\n");
    fclose(f1);
    
    // テストファイル2: 繰り返しパターン
    FILE* f2 = fopen("test_repetitive.txt", "w");
    for (int i = 0; i < 100; i++) {
        fprintf(f2, "AAAAAAAAAA");
    }
    fclose(f2);
    
    // テストファイル3: バイナリ風データ
    FILE* f3 = fopen("test_binary.bin", "wb");
    for (int i = 0; i < 256; i++) {
        unsigned char byte = (unsigned char)i;
        fwrite(&byte, 1, 1, f3);
    }
    fclose(f3);
}

int main() {
    printf("ハフマン圧縮プログラムの簡易テスト\n");
    printf("==================================\n\n");
    
    // テストファイル作成
    printf("テストファイルを作成中...\n");
    create_test_files();
    
    // 圧縮テスト
    printf("\n圧縮テストを実行中...\n");
    char* files[] = {"test_english.txt", "test_repetitive.txt", "test_binary.bin"};
    int result = compress_files(files, 3);
    
    if (result != 0) {
        printf("エラー: 圧縮テストに失敗しました\n");
        return 1;
    }
    
    // 解凍テスト
    printf("\n解凍テストを実行中...\n");
    result = decompress_file("test_english.cmp", "test_output");
    
    if (result != 0) {
        printf("エラー: 解凍テストに失敗しました\n");
        return 1;
    }
    
    printf("\n全てのテストが正常に完了しました！\n");
    printf("\n生成されたファイル:\n");
    printf("- test_english.cmp (圧縮アーカイブ)\n");
    printf("- test_output/ (解凍先ディレクトリ)\n");
    
    return 0;
}
