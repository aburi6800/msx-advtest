// License:MIT License
// copyright-holders:Hitoshi Iwai

#ifndef CONST_H
#define CONST_H

// VRAM操作用定数
#define VRAM_PTN_GENR_TBL1  0x0000  // パターンジェネレータテーブル(バンク1)
#define VRAM_PTN_GENR_TBL2  0x0800  // パターンジェネレータテーブル(バンク2)
#define VRAM_PTN_GENR_TBL3  0x1000  // パターンジェネレータテーブル(バンク3)
#define VRAM_PTN_GENR_SIZE  0x0800  // パターンジェネレータテーブルサイズ

#define VRAM_COLOR_TBL1     0x2000  // カラーテーブル(バンク1)
#define VRAM_COLOR_TBL2     0x2800  // カラーテーブル(バンク2)
#define VRAM_COLOR_TBL3     0x3000  // カラーテーブル(バンク3)
#define VRAM_COLOR_SIZE     0x0800  // カラーテーブルサイズ

#define VRAM_PTN_NAME_TBL1  0x1800  // パターンネームテーブル(バンク1)
#define VRAM_PTN_NAME_TBL2  0x1900  // パターンネームテーブル(バンク2)
#define VRAM_PTN_NAME_TBL3  0x1A00  // パターンネームテーブル(バンク3)
#define VRAM_PTN_NAME_SIZE  0xFF    // パターンネームテーブルサイズ

#define VRAM_SPR_PTN_TBL    0x3800  // スプライトパターンテーブル
#define VRAM_SPR_ATTR_TBL   0x1B00  // スプライトアトリビュートテーブル
#define VRAM_SPR_ATTR_SIZE  128     // スプライトアトリビュートテーブルサイズ

#define VRAM_WIDTH          32      // 画面幅
#define VRAM_HEIGHT         24      // 画面高さ

// MSX BIOS
#define MSX_CHSNS           0x009c  // キーボード・バッファの状態をチェック
#define MSX_CHGET           0x009f  // 1文字入力(入力待ちあり)
#define MSX_KILBUF          0x0156  // キーボード・バッファをクリア

// MSX WORKAREA
#define MSX_CLIKSW          0xf3db  // キークリックスイッチ
#define MSX_REPCNT          0xf3f7  // キーオートリピート開始までの時間
#define MSX_H_TIMI          0xfd9f  // H.TIMI割り込みフックアドレス


// 仮想画面バッファの定義
#define OFFSCR_WIDTH        32
#define OFFSCR_HEIGHT       24
#define OFFSCR_SIZE         OFFSCR_WIDTH * OFFSCR_HEIGHT

// STICK定数
#define STICK_NONE          0
#define STICK_UP            1
#define STICK_UPRIGHT       2
#define STICK_RIGHT         3
#define STICK_DOWNRIGHT     4
#define STICK_DOWN          5
#define STICK_LEFTDOWN      6
#define STICK_LEFT          7
#define STICK_UPLEFT        8

// TRIGGER定数
#define TRIGGER_ON          255
#define TRIGGER_OFF         0

#endif
