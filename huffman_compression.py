"""
ハフマン符号による圧縮・解凍プログラム

使用法:
    圧縮: python huffman_compression.py compress file1.txt [file2.txt ...]
    解凍: python huffman_compression.py decompress compressed_file.cmp [output_directory]

機能:
- 複数ファイルの一括圧縮
- ハフマン符号による高効率圧縮
- 元ファイル構造の保持
"""
import sys
import os
import time
from typing import List
from huffman_tree import HuffmanEncoder, HuffmanDecoder
from file_handler import FileHandler


def compress_files(file_paths: List[str]) -> None:
    """
    ファイルを圧縮する
    
    Args:
        file_paths: 圧縮対象ファイルのパスリスト
    """
    try:
        print(f"圧縮開始: {len(file_paths)}個のファイル")
        start_time = time.time()
        
        # ファイル存在確認
        for file_path in file_paths:
            if not os.path.exists(file_path):
                raise FileNotFoundError(f"ファイルが見つかりません: {file_path}")
        
        # ファイル読み込み
        print("ファイル読み込み中...")
        combined_data, file_boundaries = FileHandler.read_files(file_paths)
        original_size = len(combined_data)
        
        if original_size == 0:
            print("警告: 圧縮対象のデータが空です")
            return
        
        print(f"元データサイズ: {original_size:,} バイト")
        
        # ハフマン符号化
        print("ハフマン符号化中...")
        encoder = HuffmanEncoder()
        encoded_data, huffman_tree, padding = encoder.encode(combined_data)
        
        # 圧縮ファイル名生成
        output_path = FileHandler.generate_compressed_filename(file_paths)
        
        # 圧縮ファイル書き込み
        print("圧縮ファイル書き込み中...")
        FileHandler.write_compressed_file(
            output_path, 
            encoded_data, 
            huffman_tree, 
            padding,
            file_boundaries, 
            file_paths
        )
        
        # 統計情報表示
        compressed_size = os.path.getsize(output_path)
        compression_ratio = (1 - compressed_size / original_size) * 100
        end_time = time.time()
        
        print(f"\n圧縮完了!")
        print(f"出力ファイル: {output_path}")
        print(f"圧縮後サイズ: {compressed_size:,} バイト")
        print(f"圧縮率: {compression_ratio:.2f}%")
        print(f"処理時間: {end_time - start_time:.2f}秒")
        
    except Exception as e:
        print(f"圧縮エラー: {e}")
        sys.exit(1)


def decompress_file(compressed_path: str, output_dir: str = '.') -> None:
    """
    圧縮ファイルを解凍する
    
    Args:
        compressed_path: 圧縮ファイルのパス
        output_dir: 出力ディレクトリ
    """
    try:
        print(f"解凍開始: {compressed_path}")
        start_time = time.time()
        
        if not os.path.exists(compressed_path):
            raise FileNotFoundError(f"圧縮ファイルが見つかりません: {compressed_path}")
        
        # 圧縮ファイル読み込み
        print("圧縮ファイル読み込み中...")
        encoded_data, huffman_tree, padding, file_boundaries, original_files = \
            FileHandler.read_compressed_file(compressed_path)
        
        print(f"元ファイル数: {len(original_files)}")
        print(f"圧縮データサイズ: {len(encoded_data):,} バイト")
        
        # ハフマン復号化
        print("ハフマン復号化中...")
        decoder = HuffmanDecoder(huffman_tree)
        decoded_data = decoder.decode(encoded_data, padding)
        
        print(f"復号化後サイズ: {len(decoded_data):,} バイト")
        
        # ファイル抽出
        print("ファイル抽出中...")
        FileHandler.extract_files(decoded_data, file_boundaries, original_files, output_dir)
        
        end_time = time.time()
        print(f"\n解凍完了!")
        print(f"出力ディレクトリ: {os.path.abspath(output_dir)}")
        print(f"処理時間: {end_time - start_time:.2f}秒")
        
    except Exception as e:
        print(f"解凍エラー: {e}")
        sys.exit(1)


def show_usage():
    """使用法を表示"""
    print("ハフマン符号圧縮・解凍プログラム")
    print("\n使用法:")
    print("  圧縮: python huffman_compression.py compress <ファイル1> [ファイル2] ...")
    print("  解凍: python huffman_compression.py decompress <圧縮ファイル.cmp> [出力ディレクトリ]")
    print("\n例:")
    print("  python huffman_compression.py compress document.txt image.jpg")
    print("  python huffman_compression.py decompress document.cmp ./extracted/")
    print("\n注意:")
    print("  - 複数ファイルは一つの.cmpファイルにまとめられます")
    print("  - 圧縮ファイル名は最初のファイル名に.cmpを付加したものになります")
    print("  - 解凍時は元のファイル名で復元されます")


def main():
    """メイン関数"""
    if len(sys.argv) < 3:
        show_usage()
        sys.exit(1)
    
    command = sys.argv[1].lower()
    
    if command == "compress":
        # 圧縮モード
        file_paths = sys.argv[2:]
        if not file_paths:
            print("エラー: 圧縮対象ファイルが指定されていません")
            show_usage()
            sys.exit(1)
        
        compress_files(file_paths)
        
    elif command == "decompress":
        # 解凍モード
        if len(sys.argv) < 3:
            print("エラー: 圧縮ファイルが指定されていません")
            show_usage()
            sys.exit(1)
        
        compressed_file = sys.argv[2]
        output_dir = sys.argv[3] if len(sys.argv) > 3 else '.'
        
        decompress_file(compressed_file, output_dir)
        
    else:
        print(f"エラー: 不明なコマンド '{command}'")
        show_usage()
        sys.exit(1)


if __name__ == "__main__":
    main()
