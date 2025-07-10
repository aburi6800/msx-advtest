# License:MIT License
# copyright-holders:aburi6800 (Hitoshi Iwai)

import sys
import unicodedata
# MSXの文字コードマッピング
convert_map = {
    ' ' : 0x20, '!' : 0x21, '\"': 0x22, '#' : 0x23, '$' : 0x24, 
    '%' : 0x25, '&' : 0x26, '\'': 0x27, '(' : 0x28, ')' : 0x29, 
    '*' : 0x2A, '+' : 0x2B, ',' : 0x2C, '-' : 0x2D, '.' : 0x2E, 
    '/' : 0x2F, 
    '0' : 0x30, '1' : 0x31, '2' : 0x32, '3' : 0x33, '4' : 0x34, 
    '5' : 0x35, '6' : 0x36, '7' : 0x37, '8' : 0x38, '9' : 0x39, 
    ':' : 0x3A, ';' : 0x3B, '<' : 0x3C, '=' : 0x3D, '>' : 0x3E, 
    '?' : 0x3F, '@' : 0x40, 
    'A' : 0x41, 'B' : 0x42, 'C' : 0x43, 'D' : 0x44, 'E' : 0x45, 
    'F' : 0x46, 'G' : 0x47, 'H' : 0x48, 'I' : 0x49, 'J' : 0x4A, 
    'K' : 0x4B, 'L' : 0x4C, 'M' : 0x4D, 'N' : 0x4E, 'O' : 0x4F, 
    'P' : 0x50, 'Q' : 0x51, 'R' : 0x52, 'S' : 0x53, 'T' : 0x54, 
    'U' : 0x55, 'V' : 0x56, 'W' : 0x57, 'X' : 0x58, 'Y' : 0x59, 
    'Z' : 0x5A, 
    '[' : 0x5B, '\\': 0x5C, ']' : 0x5D, '^' : 0x5E, '_' : 0x5F,
    '`' : 0x60, 
    'a' : 0x61, 'b' : 0x62, 'c' : 0x63, 'd' : 0x64, 'e' : 0x65,
    'f' : 0x66, 'g' : 0x67, 'h' : 0x68, 'i' : 0x69, 'j' : 0x6A,
    'k' : 0x6B, 'l' : 0x6C, 'm' : 0x6D, 'n' : 0x6E, 'o' : 0x6F,
    'p' : 0x70, 'q' : 0x71, 'r' : 0x72, 's' : 0x73, 't' : 0x74,
    'u' : 0x75, 'v' : 0x76, 'w' : 0x77, 'x' : 0x78, 'y' : 0x79,
    'z' : 0x7A,
    'é' : 0x82,
    'á' : 0xA0, 'í' : 0xA1, 'ó' : 0xA2, 'ú' : 0xA3, 'ñ' : 0xA4,
    '¿' : 0xA8,
    '¡' : 0xAD
}

def convert_char(c):
    # 正規化で濁点・半濁点を分離
#    normalized = unicodedata.normalize('NFKD', c.upper())
    normalized = unicodedata.normalize('NFC', c)

    result = []
    for ch in normalized:
        if ch in convert_map:
            result.append(convert_map[ch])
        else:
            raise ValueError(f"未対応の文字です: '{c}'（正規化で得た文字: '{ch}'）")
    return result

def convert_string_to_msx_array(text):
    byte_array = []
    for c in text:
        byte_array.extend(convert_char(c))
    byte_array.append(0x00)  # null終端を追加
    return byte_array

def format_as_c_array(byte_array, name="str"):
    hex_values = ', '.join(f'0x{b:02X}' for b in byte_array)
    return f"uint8_t {name}[] = {{ {hex_values} }};"

# 実行エントリーポイント
if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("使い方: python msx_encode_sp.py '¡Bienvenido al mundo de MSX!'")
        sys.exit(1)

    input_text = sys.argv[1]
    try:
        result = convert_string_to_msx_array(input_text)
        print(format_as_c_array(result))
    except ValueError as e:
        print("エラー:", e)
        sys.exit(1)
