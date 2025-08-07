ハフマン符号圧縮・解凍プログラム

このプログラムは、ハフマン符号化アルゴリズムを使用してファイルを圧縮・解凍するPythonアプリケーションです。

## 機能

- **複数ファイル対応**: 複数のファイルを一つの圧縮ファイルにまとめることができます
- **ハフマン符号化**: 効率的な可変長符号化により高い圧縮率を実現
- **元ファイル構造保持**: ファイル名と境界情報を保持し、解凍時に元の構造を復元
- **バイナリファイル対応**: テキストファイルだけでなく、画像や実行ファイルなどのバイナリファイルも圧縮可能

## ファイル構成

- `huffman_compression.py`: メインプログラム（コマンドライン操作）
- `huffman_tree.py`: ハフマン木とエンコーダー/デコーダーの実装
- `file_handler.py`: ファイル操作（読み込み、書き込み、抽出）の処理

## 使用法

### 圧縮

```bash
python huffman_compression.py compress <ファイル1> [ファイル2] ...
```

例:
```bash
# 単一ファイルの圧縮
python huffman_compression.py compress document.txt

# 複数ファイルの圧縮
python huffman_compression.py compress file1.txt file2.jpg file3.pdf
```

### 解凍

```bash
python huffman_compression.py decompress <圧縮ファイル.cmp> [出力ディレクトリ]
```

例:
```bash
# 現在のディレクトリに解凍
python huffman_compression.py decompress document.cmp

# 指定ディレクトリに解凍
python huffman_compression.py decompress document.cmp ./extracted/
```

## 動作仕様

### 圧縮仕様
- 圧縮ファイル名: 最初のファイル名に`.cmp`を付加
- 複数ファイル: 一つの圧縮ファイルにまとめて保存
- ハフマン木: 全ファイルの統合データから構築
- ヘッダー情報: ファイル境界、元ファイル名、ハフマン木を保存

### 解凍仕様
- 元ファイル名での復元
- ファイル境界情報による個別ファイル分離
- バイナリデータの完全復元

## 圧縮ファイル形式

```
[4バイト: ヘッダーサイズ]
[ヘッダーデータ: pickle形式]
  - file_boundaries: ファイル境界情報
  - original_files: 元ファイル名リスト
  - huffman_tree: ハフマン木
  - padding: パディングビット数
  - data_size: 圧縮データサイズ
[圧縮データ: ハフマン符号化されたバイナリデータ]
```

## 例

### テストファイルの準備
```bash
echo "Hello, World! This is a test file for Huffman compression." > test1.txt
echo "Another test file with different content for compression testing." > test2.txt
```

### 圧縮実行
```bash
python huffman_compression.py compress test1.txt test2.txt
```

出力例:
```
圧縮開始: 2個のファイル
ファイル読み込み中...
元データサイズ: 124 バイト
ハフマン符号化中...
圧縮ファイル書き込み中...

圧縮完了!
出力ファイル: test1.cmp
圧縮後サイズ: 89 バイト
圧縮率: 28.23%
処理時間: 0.01秒
```

### 解凍実行
```bash
python huffman_compression.py decompress test1.cmp ./extracted/
```

出力例:
```
解凍開始: test1.cmp
圧縮ファイル読み込み中...
元ファイル数: 2
圧縮データサイズ: 45 バイト
ハフマン復号化中...
復号化後サイズ: 124 バイト
ファイル抽出中...
展開完了: ./extracted/test1.txt
展開完了: ./extracted/test2.txt

解凍完了!
出力ディレクトリ: C:\path\to\extracted
処理時間: 0.01秒
```

## エラーハンドリング

- ファイルが見つからない場合のエラー処理
- 圧縮ファイル形式不正の検出
- 空ファイルや不正データの処理
- メモリ不足やディスク容量不足の対応

## 制限事項

- 非常に小さなファイル（数バイト）では圧縮効果が低い場合があります
- ハフマン木の保存により、単一文字のみのファイルではファイルサイズが増加する場合があります
- 利用可能メモリ内でのファイルサイズに制限されます

## 技術的詳細

### ハフマン符号化アルゴリズム
1. 入力データの各バイト値の出現頻度を計算
2. 頻度に基づいてハフマン木を構築
3. 各バイト値に対応する可変長ビット列を生成
4. 元データを符号化し、ビット列をバイトデータに変換

### データ構造
- 優先度付きキュー（heapq）を使用したハフマン木構築
- pickle形式でのハフマン木とメタデータの保存
- バイト境界を考慮したパディング処理

## 拡張可能性

- GUI版の実装
- 進行状況表示の追加
- 複数の圧縮アルゴリズム対応
- ディレクトリ全体の再帰圧縮
- パスワード保護機能
