# ハフマン圧縮プログラム (C言語版)

このプログラムは、ハフマン符号化アルゴリズムを使用してファイルを圧縮・解凍するC言語実装です。
元のPythonバージョンから移植され、高速な処理と低メモリ使用量を実現しています。

## 特徴

- **高効率圧縮**: ハフマン符号化による最適な圧縮率
- **複数ファイル対応**: 複数のファイルを一つのアーカイブに圧縮
- **高速処理**: C言語による高速な符号化・復号化
- **クロスプラットフォーム**: Windows、Linux、macOSで動作
- **完全復元**: 元のファイル名とデータを完全に復元

## システム要件

### Windows
- GCCコンパイラ (MinGW-w64 または MSYS2推奨)
- Windows 7以降

### Linux/macOS
- GCC または Clang コンパイラ
- Make ツール

## ビルド方法

### Windows

#### 方法1: ビルドスクリプト使用
```cmd
build.bat
```

#### 方法2: 手動コンパイル
```cmd
gcc -Wall -Wextra -std=c99 -O2 -c huffman_compression.c
gcc -Wall -Wextra -std=c99 -O2 -c huffman_tree.c
gcc -Wall -Wextra -std=c99 -O2 -c file_handler.c
gcc huffman_compression.o huffman_tree.o file_handler.o -o huffman_compression.exe
```

### Linux/macOS

#### 方法1: Makefile使用
```bash
make
```

#### 方法2: 手動コンパイル
```bash
gcc -Wall -Wextra -std=c99 -O2 -c huffman_compression.c
gcc -Wall -Wextra -std=c99 -O2 -c huffman_tree.c
gcc -Wall -Wextra -std=c99 -O2 -c file_handler.c
gcc huffman_compression.o huffman_tree.o file_handler.o -o huffman_compression
```

## 使用方法

### 基本的な使用法

```bash
# 単一ファイルの圧縮
./huffman_compression compress document.txt

# 複数ファイルの圧縮
./huffman_compression compress file1.txt file2.jpg file3.pdf

# 圧縮ファイルの解凍
./huffman_compression decompress document.cmp

# 指定ディレクトリに解凍
./huffman_compression decompress document.cmp ./extracted/
```

### Windows での使用例

```cmd
REM 圧縮
huffman_compression.exe compress test.txt data.bin

REM 解凍
huffman_compression.exe decompress test.cmp restored
```

## ファイル構造

```
huffman_compression.h    - ヘッダーファイル（構造体・関数宣言）
huffman_compression.c    - メイン処理（CLI、統計表示）
huffman_tree.c          - ハフマン木実装（符号化・復号化）
file_handler.c          - ファイル操作（読み書き・アーカイブ）
Makefile               - Linux/macOS用ビルド設定
build.bat              - Windows用ビルドスクリプト
README_C.md            - このファイル
```

## アルゴリズム詳細

### ハフマン符号化の流れ

1. **頻度分析**: 入力データの各バイト値の出現頻度を計算
2. **ハフマン木構築**: 最小ヒープを使用して最適な符号木を生成
3. **符号テーブル生成**: 各文字に対する可変長ビット符号を生成
4. **データ符号化**: 元データを符号テーブルに従ってビット列に変換
5. **アーカイブ作成**: ヘッダー、ツリー情報、圧縮データを結合

### 圧縮ファイル形式

```
[ヘッダーサイズ(4バイト)] [ヘッダー構造体] [シリアライズされたツリー] [圧縮データ]
```

ヘッダー構造体:
- ファイル数
- パディングビット数
- ツリーデータサイズ
- 圧縮データサイズ
- ファイル境界情報（各ファイルの開始位置とサイズ）

## パフォーマンス比較

| 項目 | Python版 | C言語版 |
|------|----------|---------|
| 圧縮速度 | 1x | 約5-10x |
| 解凍速度 | 1x | 約8-15x |
| メモリ使用量 | 高 | 低 |
| ファイルサイズ | 大 | 小 |

## テスト

### 自動テスト実行

#### Windows
```cmd
build.bat
```
（ビルドスクリプトにテストが含まれています）

#### Linux/macOS
```bash
make test
```

### 手動テスト

```bash
# テストファイル作成
echo "Hello, World!" > test.txt

# 圧縮
./huffman_compression compress test.txt

# ファイルサイズ確認
ls -la test.txt test.cmp

# 解凍
./huffman_compression decompress test.cmp extracted

# 結果確認
diff test.txt extracted/test.txt
```

## トラブルシューティング

### コンパイルエラー

**エラー**: `gcc: command not found`
**解決**: GCCコンパイラをインストール
- Windows: MinGW-w64 または MSYS2
- Ubuntu/Debian: `sudo apt install gcc`
- CentOS/RHEL: `sudo yum install gcc`
- macOS: Xcode Command Line Tools

**エラー**: `No such file or directory`
**解決**: 全てのソースファイルが同じディレクトリにあることを確認

### 実行時エラー

**エラー**: `ファイルが見つかりません`
**解決**: 
- ファイルパスが正しいことを確認
- ファイル読み取り権限があることを確認

**エラー**: `メモリ確保に失敗しました`
**解決**: 
- 十分なメモリがあることを確認
- 大きなファイルの場合は分割を検討

## 制限事項

- 最大ファイル数: 100個
- 最大ファイル名長: 255文字
- メモリ使用量: 入力データサイズの約2-3倍

## ライセンス

このソフトウェアはMITライセンスの下で配布されています。

## 貢献

バグ報告、機能要求、プルリクエストを歓迎します。

## 更新履歴

### v1.0.0
- 初期リリース
- Python版からの完全移植
- Windows/Linux/macOS サポート
- 自動テスト機能
