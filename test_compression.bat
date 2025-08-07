@echo off
REM ハフマン圧縮テストスクリプト
echo ===== ハフマン圧縮・解凍テスト =====
echo.

REM テストファイルの存在確認
if not exist test1.txt (
    echo エラー: test1.txt が見つかりません
    pause
    exit /b 1
)
if not exist test2.txt (
    echo エラー: test2.txt が見つかりません
    pause
    exit /b 1
)

echo 1. 元ファイルのサイズを確認
dir test*.txt

echo.
echo 2. 複数ファイルを圧縮中...
python huffman_compression.py compress test1.txt test2.txt

echo.
echo 3. 圧縮ファイルのサイズを確認
if exist test1.cmp (
    dir test1.cmp
) else (
    echo 圧縮ファイルが作成されませんでした
    pause
    exit /b 1
)

echo.
echo 4. extracted フォルダを作成
if exist extracted rmdir /s /q extracted
mkdir extracted

echo.
echo 5. 圧縮ファイルを解凍中...
python huffman_compression.py decompress test1.cmp extracted

echo.
echo 6. 解凍結果を確認
if exist extracted (
    echo 解凍されたファイル:
    dir extracted
    echo.
    echo 元ファイルとの比較:
    echo --- test1.txt vs extracted\test1.txt ---
    fc /b test1.txt extracted\test1.txt
    echo --- test2.txt vs extracted\test2.txt ---
    fc /b test2.txt extracted\test2.txt
) else (
    echo 解凍フォルダが作成されませんでした
)

echo.
echo ===== テスト完了 =====
pause
