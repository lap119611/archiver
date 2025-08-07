@echo off
REM Windows用のビルドスクリプト

echo ハフマン圧縮プログラム (C言語版) のビルド
echo ===============================================

REM コンパイラの存在確認
gcc --version >nul 2>&1
if errorlevel 1 (
    echo エラー: GCCコンパイラが見つかりません
    echo MinGW-w64やMSYS2をインストールしてPATHに追加してください
    pause
    exit /b 1
)

echo GCCコンパイラが見つかりました

REM オブジェクトファイルの削除
if exist *.o del *.o
if exist huffman_compression.exe del huffman_compression.exe

echo 既存のファイルをクリーンアップしました

REM コンパイル
echo コンパイル中...
gcc -Wall -Wextra -std=c99 -O2 -c huffman_compression.c -o huffman_compression.o
if errorlevel 1 (
    echo エラー: huffman_compression.cのコンパイルに失敗しました
    pause
    exit /b 1
)

gcc -Wall -Wextra -std=c99 -O2 -c huffman_tree.c -o huffman_tree.o
if errorlevel 1 (
    echo エラー: huffman_tree.cのコンパイルに失敗しました
    pause
    exit /b 1
)

gcc -Wall -Wextra -std=c99 -O2 -c file_handler.c -o file_handler.o
if errorlevel 1 (
    echo エラー: file_handler.cのコンパイルに失敗しました
    pause
    exit /b 1
)

echo リンク中...
gcc huffman_compression.o huffman_tree.o file_handler.o -o huffman_compression.exe
if errorlevel 1 (
    echo エラー: リンクに失敗しました
    pause
    exit /b 1
)

echo ビルド完了！

REM テストファイルの作成と実行
echo.
echo 簡単なテストを実行します...

REM テストファイルの作成
echo Hello, World! This is a test file for Huffman compression. > test_sample.txt
echo The quick brown fox jumps over the lazy dog. >> test_sample.txt
echo 1234567890 abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ >> test_sample.txt

echo これは日本語のテストファイルです。ハフマン圧縮のテストを行います。 > test_japanese.txt
echo 圧縮率の確認と文字化けの有無をチェックします。 >> test_japanese.txt

REM 圧縮テスト
echo.
echo 圧縮テストを実行中...
huffman_compression.exe compress test_sample.txt test_japanese.txt
if errorlevel 1 (
    echo エラー: 圧縮テストに失敗しました
    pause
    exit /b 1
)

REM 解凍テスト
echo.
echo 解凍テストを実行中...
huffman_compression.exe decompress test_sample.cmp extracted
if errorlevel 1 (
    echo エラー: 解凍テストに失敗しました
    pause
    exit /b 1
)

echo.
echo ==============================================
echo ビルドとテストが正常に完了しました！
echo 実行ファイル: huffman_compression.exe
echo ==============================================
echo.
echo 使用方法:
echo   圧縮: huffman_compression compress file1.txt [file2.txt ...]
echo   解凍: huffman_compression decompress archive.cmp [output_dir]
echo.
echo 例:
echo   huffman_compression compress document.txt
echo   huffman_compression decompress document.cmp ./restored/
echo.
pause
