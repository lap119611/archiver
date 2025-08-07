"""
大きなテストファイル生成スクリプト
"""

def create_large_test_file():
    """大きなテストファイルを作成"""
    
    # 繰り返しの多いコンテンツを作成（より現実的な圧縮テスト）
    content = []
    
    # よく使われる英単語の繰り返し
    common_words = [
        "the", "and", "for", "are", "but", "not", "you", "all", 
        "can", "had", "her", "was", "one", "our", "out", "day",
        "get", "has", "him", "his", "how", "its", "may", "new",
        "now", "old", "see", "two", "who", "boy", "did", "man"
    ]
    
    # 1000回繰り返して大きなファイルを作成
    for i in range(1000):
        line = f"Line {i:04d}: "
        # 頻度の偏りを作るため、いくつかの単語を多用
        for j in range(10):
            if j % 3 == 0:
                line += "the the the "  # 高頻度
            elif j % 3 == 1:
                line += "and and "      # 中頻度
            else:
                line += f"{common_words[i % len(common_words)]} "  # 低頻度
        
        content.append(line + "\n")
    
    # ファイルに書き込み
    with open("large_test.txt", "w", encoding="utf-8") as f:
        f.writelines(content)
    
    print(f"大きなテストファイルを作成しました: large_test.txt")
    print(f"ファイルサイズ: {len(''.join(content))} バイト")

if __name__ == "__main__":
    create_large_test_file()
