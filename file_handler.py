"""
ファイル操作ハンドラ
複数ファイルの処理と圧縮ファイルの保存/読み込みを担当
"""
import os
import pickle
import struct
from typing import List, Tuple, Dict, Any
from huffman_tree import HuffmanNode


class FileHandler:
    """ファイル操作を管理するクラス"""
    
    @staticmethod
    def read_files(file_paths: List[str]) -> Tuple[bytes, Dict[str, int]]:
        """
        複数ファイルを読み込み、連結したデータとファイル境界情報を返す
        
        Args:
            file_paths: ファイルパスのリスト
            
        Returns:
            連結されたファイルデータとファイル境界情報
        """
        combined_data = bytearray()
        file_boundaries = {}
        current_pos = 0
        
        for file_path in file_paths:
            if not os.path.exists(file_path):
                raise FileNotFoundError(f"ファイルが見つかりません: {file_path}")
            
            with open(file_path, 'rb') as f:
                file_data = f.read()
                
            file_boundaries[os.path.basename(file_path)] = {
                'start': current_pos,
                'size': len(file_data)
            }
            
            combined_data.extend(file_data)
            current_pos += len(file_data)
        
        return bytes(combined_data), file_boundaries
    
    @staticmethod
    def write_compressed_file(output_path: str, 
                            encoded_data: bytes, 
                            huffman_tree: HuffmanNode,
                            padding: int,
                            file_boundaries: Dict[str, int],
                            original_files: List[str]) -> None:
        """
        圧縮ファイルを書き込む
        
        Args:
            output_path: 出力ファイルパス
            encoded_data: 符号化されたデータ
            huffman_tree: ハフマン木
            padding: パディングビット数
            file_boundaries: ファイル境界情報
            original_files: 元ファイル名のリスト
        """
        with open(output_path, 'wb') as f:
            # ヘッダー情報を書き込み
            header = {
                'file_boundaries': file_boundaries,
                'original_files': [os.path.basename(path) for path in original_files],
                'huffman_tree': huffman_tree,
                'padding': padding,
                'data_size': len(encoded_data)
            }
            
            # ヘッダーをpickleでシリアライズ
            header_data = pickle.dumps(header)
            header_size = len(header_data)
            
            # ヘッダーサイズを書き込み（4バイト）
            f.write(struct.pack('>I', header_size))
            
            # ヘッダーデータを書き込み
            f.write(header_data)
            
            # 圧縮データを書き込み
            f.write(encoded_data)
    
    @staticmethod
    def read_compressed_file(file_path: str) -> Tuple[bytes, HuffmanNode, int, Dict[str, Any], List[str]]:
        """
        圧縮ファイルを読み込む
        
        Args:
            file_path: 圧縮ファイルパス
            
        Returns:
            圧縮データ、ハフマン木、パディング、ファイル境界情報、元ファイル名リスト
        """
        with open(file_path, 'rb') as f:
            # ヘッダーサイズを読み込み
            header_size_data = f.read(4)
            if len(header_size_data) != 4:
                raise ValueError("不正な圧縮ファイル形式です")
            
            header_size = struct.unpack('>I', header_size_data)[0]
            
            # ヘッダーデータを読み込み
            header_data = f.read(header_size)
            if len(header_data) != header_size:
                raise ValueError("ヘッダーデータが不完全です")
            
            # ヘッダーをデシリアライズ
            header = pickle.loads(header_data)
            
            # 圧縮データを読み込み
            encoded_data = f.read(header['data_size'])
            if len(encoded_data) != header['data_size']:
                raise ValueError("圧縮データが不完全です")
            
            return (encoded_data, 
                   header['huffman_tree'], 
                   header['padding'],
                   header['file_boundaries'], 
                   header['original_files'])
    
    @staticmethod
    def extract_files(decoded_data: bytes, 
                     file_boundaries: Dict[str, Any], 
                     original_files: List[str],
                     output_dir: str = '.') -> None:
        """
        復号化されたデータから個別ファイルを抽出
        
        Args:
            decoded_data: 復号化されたデータ
            file_boundaries: ファイル境界情報
            original_files: 元ファイル名リスト
            output_dir: 出力ディレクトリ
        """
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)
        
        for filename in original_files:
            if filename in file_boundaries:
                boundary = file_boundaries[filename]
                start = boundary['start']
                size = boundary['size']
                
                file_data = decoded_data[start:start + size]
                
                output_path = os.path.join(output_dir, filename)
                with open(output_path, 'wb') as f:
                    f.write(file_data)
                
                print(f"展開完了: {output_path}")
    
    @staticmethod
    def generate_compressed_filename(file_paths: List[str]) -> str:
        """
        圧縮ファイル名を生成
        
        Args:
            file_paths: 元ファイルパスのリスト
            
        Returns:
            圧縮ファイル名
        """
        if not file_paths:
            raise ValueError("ファイルパスが指定されていません")
        
        # 最初のファイル名を基に圧縮ファイル名を生成
        first_file = file_paths[0]
        base_name = os.path.splitext(first_file)[0]
        return f"{base_name}.cmp"
