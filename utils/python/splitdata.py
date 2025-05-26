# License:MIT License
# copyright-holders:aburi6800 (Hitoshi Iwai)

# 画像は19x14キャラクター = 152x112ドット
# 8バイト = 1キャラクター データの並びは以下となる
#   chr1  chr2  ...
#   +0    +8    ...
#   +1    +9    ...
#   +2    +a    ...
#   +3    +b    ...
#   +4    +c    ...
#   +5    +d    ...
#   +6    +e    ...
#   +7    +f    ...
# - なので、8バイト単位で処理する
# - パターンジェネレータテーブル、カラーテーブルそれぞれについて、以下を処理
#   - バンク0
#     - 0行目～7行目の8回ループ
#       - 8バイト x 19 = 152バイトを読んで出力
#       - 8バイト x 13 = 104バイトを読み飛ばす
#   - バンク1
#     - 0行目～5行目の6回ループ
#       - 8バイト x 19 = 152バイトを読んで出力
#       - 8バイト x 13 = 104バイトを読み飛ばす
import sys
import os

def process_bank(data: bytes, rows: int, chars_per_row: int = 19, skip_chars: int = 13):
    """
    バンクデータ処理関数
    :param data: 入力バイナリデータ
    :param rows: 処理する行数（バンク0なら8、バンク1なら6）
    :param chars_per_row: 1行あたりのキャラクタ数（19）
    :param skip_chars: スキップするキャラクタ数（13）
    :return: 処理後のデータ bytes
    """
    processed = bytearray()
    offset = 0
    for row in range(rows):
        read_size = 8 * chars_per_row
        processed += data[offset:offset + read_size]
        offset += read_size

        skip_size = 8 * skip_chars
        offset += skip_size

    return bytes(processed)


def main():
    if len(sys.argv) < 2:
        print("使い方: python splitdata.py <input_file>")
        sys.exit(1)

    input_filename = sys.argv[1]
    base, ext = os.path.splitext(input_filename)
    if not ext:
        print("警告: 入力ファイルに拡張子がありません。出力は .bin になります。")
        ext = ".bin"  # 拡張子なしのまま処理

    output_filename_bank0 = f"{base}_blk0{ext}"
    output_filename_bank1 = f"{base}_blk1{ext}"

    # 入力ファイル読み込み
    with open(input_filename, "rb") as f:
        raw_data = f.read()

    # 先頭7バイトをスキップ
    if len(raw_data) <= 7:
        print("エラー: 入力ファイルが小さすぎます（先頭7バイトをスキップできません）")
        sys.exit(1)

    data = raw_data[7:]

    # バンク0の処理（8行）
    bank0_data = process_bank(data, rows=8)

    # バンク1の処理（バンク0相当のデータをスキップしてから）
    bank1_offset = 8 * (19 + 13) * 8  # 2048バイト
    bank1_data = process_bank(data[bank1_offset:], rows=6)

    # 出力
    with open(output_filename_bank0, "wb") as f:
        f.write(bank0_data)

    with open(output_filename_bank1, "wb") as f:
        f.write(bank1_data)

    print(f"出力完了: {output_filename_bank0}, {output_filename_bank1}")


if __name__ == "__main__":
    main()
