"""
ハフマン符号化のためのツリー実装
"""
import heapq
from collections import defaultdict, Counter
from typing import Dict, Optional, Tuple, Any
import pickle


class HuffmanNode:
    """ハフマン木のノード"""
    
    def __init__(self, char: Optional[str] = None, freq: int = 0, 
                 left: Optional['HuffmanNode'] = None, 
                 right: Optional['HuffmanNode'] = None):
        self.char = char
        self.freq = freq
        self.left = left
        self.right = right
    
    def __lt__(self, other: 'HuffmanNode') -> bool:
        """ヒープでの比較のため"""
        return self.freq < other.freq
    
    def is_leaf(self) -> bool:
        """葉ノードかどうかを判定"""
        return self.left is None and self.right is None


class HuffmanEncoder:
    """ハフマン符号化器"""
    
    def __init__(self):
        self.root: Optional[HuffmanNode] = None
        self.codes: Dict[str, str] = {}
        self.reverse_codes: Dict[str, str] = {}
    
    def build_frequency_table(self, data: bytes) -> Dict[int, int]:
        """頻度表を作成"""
        return Counter(data)
    
    def build_huffman_tree(self, freq_table: Dict[int, int]) -> HuffmanNode:
        """ハフマン木を構築"""
        if not freq_table:
            raise ValueError("空の頻度表です")
        
        # 特殊ケース：1文字のみの場合
        if len(freq_table) == 1:
            char = list(freq_table.keys())[0]
            root = HuffmanNode(freq=freq_table[char])
            root.left = HuffmanNode(char=chr(char), freq=freq_table[char])
            return root
        
        # ヒープにノードを追加
        heap = []
        for byte_val, freq in freq_table.items():
            node = HuffmanNode(char=chr(byte_val), freq=freq)
            heapq.heappush(heap, node)
        
        # ハフマン木を構築
        while len(heap) > 1:
            left = heapq.heappop(heap)
            right = heapq.heappop(heap)
            
            merged = HuffmanNode(freq=left.freq + right.freq, left=left, right=right)
            heapq.heappush(heap, merged)
        
        return heap[0]
    
    def generate_codes(self, root: HuffmanNode) -> Dict[str, str]:
        """ハフマン符号を生成"""
        if root is None:
            return {}
        
        codes = {}
        
        def traverse(node: HuffmanNode, code: str = ""):
            if node.is_leaf():
                # 特殊ケース：ルートが葉の場合
                codes[node.char] = code if code else "0"
            else:
                if node.left:
                    traverse(node.left, code + "0")
                if node.right:
                    traverse(node.right, code + "1")
        
        traverse(root)
        return codes
    
    def encode(self, data: bytes) -> Tuple[bytes, HuffmanNode]:
        """データを符号化"""
        if not data:
            raise ValueError("空のデータです")
        
        # 頻度表作成
        freq_table = self.build_frequency_table(data)
        
        # ハフマン木構築
        self.root = self.build_huffman_tree(freq_table)
        
        # 符号生成
        self.codes = self.generate_codes(self.root)
        self.reverse_codes = {v: k for k, v in self.codes.items()}
        
        # データを符号化
        encoded_bits = ""
        for byte_val in data:
            char = chr(byte_val)
            encoded_bits += self.codes[char]
        
        # ビット列をバイトに変換
        # パディングビット数を記録
        padding = 8 - len(encoded_bits) % 8
        if padding != 8:
            encoded_bits += "0" * padding
        
        encoded_bytes = bytearray()
        for i in range(0, len(encoded_bits), 8):
            byte_str = encoded_bits[i:i+8]
            encoded_bytes.append(int(byte_str, 2))
        
        return bytes(encoded_bytes), self.root, padding


class HuffmanDecoder:
    """ハフマン復号化器"""
    
    def __init__(self, root: HuffmanNode):
        self.root = root
    
    def decode(self, encoded_data: bytes, padding: int) -> bytes:
        """符号化データを復号化"""
        if not encoded_data:
            return b""
        
        # バイトをビット列に変換
        bit_string = ""
        for byte_val in encoded_data:
            bit_string += format(byte_val, '08b')
        
        # パディングを除去
        if padding > 0:
            bit_string = bit_string[:-padding]
        
        # ハフマン木を使って復号化
        decoded_bytes = bytearray()
        current_node = self.root
        
        for bit in bit_string:
            if current_node.is_leaf():
                # 特殊ケース：ルートが葉の場合
                decoded_bytes.append(ord(current_node.char))
                current_node = self.root
            
            if bit == "0":
                current_node = current_node.left
            else:
                current_node = current_node.right
            
            if current_node and current_node.is_leaf():
                decoded_bytes.append(ord(current_node.char))
                current_node = self.root
        
        return bytes(decoded_bytes)
