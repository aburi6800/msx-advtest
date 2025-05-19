// License:MIT License
// copyright-holders:Hitoshi Iwai
// Compiler     : zcc (z88dk)
// Include path : {$Z88DK_HOME}/include/* , {$Z88DK_HOME}/include/**/*

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <msx.h>
#include <msx/gfx.h>

#include "../include/resource.h"
#include "../include/const.h"
#include "../include/unpack.h"
#include "../include/switch_bank.h"
#include "../include/message.h"
#include "../include/key.h"


// 最大シーン数
#define MAX_SCENES 10

// 最大選択肢数
#define MAX_CHOICES 16

// フラグ用定義
#define FLAG_FOUND_KEY   (1 << 0)  // 鍵を見つけた
#define FLAG_HAVE_KEY    (1 << 1)  // 鍵を手に入れた
#define FLAG_OPEN_DOOR   (1 << 2)  // ドアを開けた
#define FLAG_POLISHED    (1 << 3)  // 柱を磨いた
//#define FLAG_HAVE_CROTH  (1 << 3)  // 赤い布を手に入れた
//#define FLAG_HAVE_HAMMER (1 << 4)  // ハンマーを手に入れた
//#define FLAG_HAVE_SWORD  (1 << 5)  // 剣を手に入れた


// font.asmへの参照
extern uint8_t FONT_COL_TBL[];
extern uint8_t FONT_PTN_TBL[];


// 選択肢定義
typedef struct {
    const char *command;            // プレイヤーの入力コマンド
    uint8_t required_flag;          // この選択肢が表示・実行できる条件(0で条件なし)
    uint8_t flag_to_check;          // メッセージの分岐に使うフラグ(0で分岐なし)
    uint8_t *message_if_unset;      // フラグが未設定のときの表示メッセージ
    uint8_t set_flag_if_unset;      // 実行後にセットするフラグ(フラグ未設定時)
    uint8_t next_scene_if_unset;    // 次のシーンID(0で遷移なし)
    uint8_t *message_if_set;        // フラグが設定済のときの表示メッセージ
    uint8_t set_flag_if_set;        // 実行後にセットするフラグ(フラグ設定時)
    uint8_t next_scene_if_set;      // 次のシーンID(0で遷移なし)
} Choice;


// シーン定義
typedef struct {
    uint8_t id;
    uint8_t flag_to_check;          // シーンの分岐に使うフラグ(0で分岐なし)
    uint8_t next_scene_if_unset;    // フラグが未設定のときのシーンID
    uint8_t next_scene_if_set;      // フラグが設定済のときのシーンID
    uint8_t graphic_bank;           // グラフィックデータのバンク
    uint8_t *graphic_ptn0;          // グラフィックデータ(パターンジェネレータテーブル BANK0)
    uint8_t *graphic_ptn1;          // グラフィックデータ(パターンジェネレータテーブル BANK1)
    uint8_t *graphic_col0;          // グラフィックデータ(カラーテーブル BANK0)
    uint8_t *graphic_col1;          // グラフィックデータ(カラーテーブル BANK1)
    uint8_t *message;               // シーンの最初に表示するメッセージ
    Choice choices[MAX_CHOICES];
} Scene;


// メッセージデータ

// プロンプトメッセージ
uint8_t promptMessage[]  = {0x43, 0x4f, 0x4d, 0x4d, 0x41, 0x4e, 0x44, 0x3f, 0x00};
// 実行不可コマンドメッセージ
uint8_t dontMessage[]  = {0x9f, 0xfa, 0xea, 0x20, 0xe3, 0xde, 0x97, 0xef, 0x9e, 0xfd, 0xa1, 0x00};
// 入力待ちメッセージ
uint8_t waitMessage[]  = {0x5b, 0x50, 0x55, 0x53, 0x48, 0x20, 0x53, 0x50, 0x41, 0x43, 0x45, 0x5d, 0x00};
// ブランク文字
uint8_t *blank = " ";
// デバッグ用文字
uint8_t *debug = "*";

// あなたは小さな部屋にいる。
uint8_t message0100[] = {0x91, 0xe5, 0xe0, 0xea, 0xa4, 0xe1, 0x92, 0x9b, 0xe5, 0xed, 0xf4, 0x28, 0x52, 0x4f, 0x4f, 0x4d, 0x29, 0xe6, 0x92, 0xf9, 0xa1, 0x00};
// あなたは周囲を見渡した。扉(door)とテーブル(table)がある。
uint8_t message0101[] = {0x91, 0xe5, 0xe0, 0xea, 0x20, 0x9c, 0x8d, 0x93, 0x92, 0x86, 0x20, 0xf0, 0xfc, 0xe0, 0x9c, 0xe0, 0xa1, 0x0a, 0xc4, 0xde, 0xb1, 0x28, 0x44, 0x4f, 0x4f, 0x52, 0x29, 0xe4, 0x20, 0xc3, 0xb0, 0xcc, 0xde, 0xd9, 0x28, 0x54, 0x41, 0x42, 0x4c, 0x45, 0x29, 0x96, 0xde, 0x20, 0x91, 0xf9, 0xa1, 0x00};
// テーブルの下に鍵(key)を見つけた。
uint8_t message0102[] = {0xc3, 0xb0, 0xcc, 0xde, 0xd9, 0xe9, 0x9c, 0xe0, 0xe6, 0xa4, 0xb6, 0xb7, 0xde, 0x28, 0x4b, 0x45, 0x59, 0x29, 0x86, 0x20, 0xf0, 0xe2, 0x99, 0xe0, 0xa1, 0x00};
// テーブルの周りには、もう何もない。
uint8_t message0103[] = {0xc3, 0xb0, 0xcc, 0xde, 0xd9, 0xe9, 0x20, 0xef, 0xfc, 0xf8, 0xe6, 0xea, 0xa4, 0xf3, 0x93, 0x20, 0xe5, 0xe6, 0xf3, 0xe5, 0x92, 0xa1, 0x00};
// 鍵を手に入れた！
uint8_t message0104[] = {0xb6, 0xb7, 0xde, 0x86, 0x20, 0xe3, 0xe6, 0x92, 0xfa, 0xe0, 0x21, 0x00};
// 既に持っている。
uint8_t message0105[] = {0x9d, 0xe3, 0xde, 0xe6, 0x20, 0xf3, 0x8f, 0xe3, 0x92, 0xf9, 0xa1, 0x00};
// 鍵がかかっている。
uint8_t message0106[] = {0xb6, 0xb7, 0xde, 0x96, 0xde, 0x20, 0x96, 0x96, 0x8f, 0xe3, 0x92, 0xf9, 0xa1, 0x00};
// 扉は開いている。
uint8_t message0107[] = {0xe4, 0xeb, 0xde, 0xf7, 0xea, 0x20, 0x91, 0x92, 0xe3, 0x92, 0xf9, 0xa1, 0x00};
// ドアには鍵がかかっていて、開けることができない。
uint8_t message0108[] = {0xc4, 0xde, 0xb1, 0xe6, 0xea, 0xb6, 0xb7, 0xde, 0x96, 0xde, 0x96, 0x96, 0x8f, 0xe3, 0x92, 0xe3, 0xa4, 0x91, 0x99, 0xf9, 0x9a, 0xe4, 0x96, 0xde, 0x20, 0xe3, 0xde, 0x97, 0xe5, 0x92, 0xa1, 0x00};
// 鍵を使ってドアを開けた。
uint8_t message0109[] = {0xb6, 0xb7, 0xde, 0x86, 0xe2, 0x96, 0x8f, 0xe3, 0xa4, 0xc4, 0xde, 0xb1, 0x86, 0x91, 0x99, 0xe0, 0xa1, 0x00};
// ドアの外に出ます。
uint8_t message010a[] = {0xc4, 0xde, 0xb1, 0xe9, 0x9f, 0xe4, 0xe6, 0x20, 0xe3, 0xde, 0xef, 0x9d, 0xa1, 0x00};
// 少し錆びている。古い鍵のようだ。
uint8_t message010b[] = {0x9d, 0x9a, 0x9c, 0x20, 0x9b, 0xeb, 0xde, 0xe3, 0x92, 0xf9, 0xa1, 0x0a, 0xec, 0xf9, 0x92, 0xb6, 0xb7, 0xde, 0xe9, 0xf6, 0x93, 0xe0, 0xde, 0xa1, 0x00};


// 部屋の中心に、大きな柱がある。左右の壁には扉がある。
uint8_t message0200[] = {0xed, 0xf4, 0xe9, 0x20, 0xe1, 0x8c, 0x93, 0x9c, 0xfd, 0xe6, 0xa4, 0x95, 0x95, 0x97, 0xe5, 0x20, 0xea, 0x9c, 0xf7, 0x28, 0x50, 0x49, 0x4c, 0x4c, 0x45, 0x52, 0x29, 0x96, 0xde, 0x91, 0xf9, 0xa1, 0x0a, 0x9b, 0xf5, 0x93, 0xe9, 0x96, 0xed, 0xde, 0xe6, 0xea, 0xa4, 0xe4, 0xeb, 0xde, 0xf7, 0x28, 0x44, 0x4f, 0x4f, 0x52, 0x29, 0x96, 0xde, 0x20, 0x91, 0xf9, 0xa1, 0x00};
// どちらの扉も、鍵はかかっていないようだ。
uint8_t message0201[] = {0xe4, 0xde, 0xe1, 0xf7, 0xe9, 0xe4, 0xeb, 0xde, 0xf7, 0xf3, 0xa4, 0xb6, 0xb7, 0xde, 0xea, 0x96, 0x96, 0x8f, 0xe3, 0x92, 0xe5, 0x92, 0xf6, 0x93, 0xe0, 0xde, 0xa1, 0x00};
// 太い柱だ。少し汚れている。
uint8_t message0202[] = {0xec, 0xe4, 0x92, 0x20, 0xea, 0x9c, 0xf7, 0xe0, 0xde, 0xa1, 0x0a, 0x9d, 0x9a, 0x9c, 0x20, 0xf6, 0x9a, 0xde, 0xfa, 0xe3, 0x92, 0xf9, 0xa1, 0x00};
// 柱が少しきれいになった。
uint8_t message0203[] = {0xea, 0x9c, 0xf7, 0x96, 0xde, 0x9d, 0x9a, 0x9c, 0xa4, 0x97, 0xfa, 0x92, 0xe6, 0xe5, 0x8f, 0xe0, 0xa1, 0x00};
// 柱が輝きだしだ！あなたは光につつまれてゆく．．．
uint8_t message0204[] = {0xea, 0x9c, 0xf7, 0x96, 0xde, 0x20, 0x96, 0x96, 0xde, 0xf4, 0x97, 0xe0, 0xde, 0x9c, 0xe0, 0x21, 0x0a, 0x91, 0xe5, 0xe0, 0xea, 0xa4, 0xeb, 0x96, 0xf8, 0xe6, 0x20, 0xe2, 0xe2, 0xef, 0xfa, 0xe3, 0xf5, 0x98, 0xa5, 0xa5, 0xa5, 0xa1, 0x00};


// ガイコツだ！あなたを見つけると、ゆっくりと近付いてきた。
uint8_t message0300[] = {0xb6, 0xde, 0xb2, 0xba, 0xc2, 0x28, 0x53, 0x4B, 0x45, 0x4c, 0x54, 0x4f, 0x4e, 0x29, 0xe0, 0xde, 0x21, 0x0a, 0x91, 0xe5, 0xe0, 0x86, 0xf0, 0xe2, 0x99, 0xf9, 0xe4, 0xa4, 0xf5, 0x8f, 0x98, 0xf8, 0xe4, 0x20, 0xe1, 0x96, 0xe2, 0xde, 0x92, 0xe3, 0x97, 0xe0, 0xa1, 0x00};
// 剣を持っており、首には赤い布を掛けている。
uint8_t message0301[] = {0x99, 0xfd, 0x28, 0x53, 0x57, 0x4f, 0x52, 0x44, 0x29, 0x86, 0xf3, 0x8f, 0xe3, 0x92, 0xe3, 0xa4, 0x98, 0xeb, 0xde, 0xe6, 0xea, 0x91, 0x96, 0x92, 0xe7, 0xe9, 0x28, 0x43, 0x4c, 0x4f, 0x54, 0x48, 0x29, 0x86, 0x96, 0x99, 0xe3, 0x92, 0xf9, 0xa1, 0x00};

// あなたは部屋から脱出した。ゲームクリア！
uint8_t message0400[] = {0x91, 0xe5, 0xe0, 0xea, 0xa4, 0xed, 0xf4, 0x96, 0xf7, 0x20, 0xe0, 0xde, 0x8f, 0x9c, 0x8d, 0xe2, 0x9c, 0xe0, 0xa1, 0x0a, 0xb9, 0xb0, 0xd1, 0xb8, 0xd8, 0xb1, 0x21, 0x00};


// シーンデータ
Scene scenes[MAX_SCENES] = {
    // ROOM 1
    {
        .id = 0,
        .flag_to_check       = 0,
        .next_scene_if_unset = 0,
        .next_scene_if_set   = 0,
        .graphic_bank        = 1,
        .graphic_ptn0        = PTN_ROOM1_0,
        .graphic_ptn1        = PTN_ROOM1_1,
        .graphic_col0        = COL_ROOM1_0,
        .graphic_col1        = COL_ROOM1_1,
        .message             = message0100,
        .choices = {
            {
                .command = "LOOK ROOM",
                .required_flag       = 0,
                .flag_to_check       = 0,
                .message_if_unset    = message0101,
                .set_flag_if_unset   = 0,
                .next_scene_if_unset = 0,
                .message_if_set      = "",
                .set_flag_if_set     = 0,
                .next_scene_if_set   = 0
            },
            {
                .command = "LOOK TABLE",
                .required_flag       = 0,
                .flag_to_check       = FLAG_HAVE_KEY,
                .message_if_unset    = message0102,
                .set_flag_if_unset   = FLAG_FOUND_KEY,
                .next_scene_if_unset = 0,
                .message_if_set      = message0103,
                .set_flag_if_set     = 0,
                .next_scene_if_set   = 0
            },
            {
                .command = "LOOK KEY",
                .required_flag       = FLAG_FOUND_KEY,
                .flag_to_check       = 0,
                .message_if_unset    = message010b,
                .set_flag_if_unset   = 0,
                .next_scene_if_unset = 0,
                .message_if_set      = "",
                .set_flag_if_set     = 0,
                .next_scene_if_set   = 0
            },
            {
                .command = "GET KEY",
                .required_flag       = FLAG_FOUND_KEY,
                .flag_to_check       = FLAG_HAVE_KEY,
                .message_if_unset    = message0104,
                .set_flag_if_unset   = FLAG_HAVE_KEY,
                .next_scene_if_unset = 0,
                .message_if_set      = message0105,
                .set_flag_if_set     = 0,
                .next_scene_if_set   = 0
            },
            {
                .command = "LOOK DOOR",
                .required_flag       = 0,
                .flag_to_check       = FLAG_OPEN_DOOR,
                .message_if_unset    = message0106,
                .set_flag_if_unset   = 0,
                .next_scene_if_unset = 0,
                .message_if_set      = message0107,
                .set_flag_if_set     = 0,
                .next_scene_if_set   = 0
            },
            {
                .command = "OPEN DOOR",
                .required_flag       = 0,
                .flag_to_check       = FLAG_HAVE_KEY,
                .message_if_unset    = message0108,
                .set_flag_if_unset   = 0,
                .next_scene_if_unset = 0,
                .message_if_set      = message0109,
                .set_flag_if_set     = FLAG_OPEN_DOOR,
                .next_scene_if_set = 0
            },
            {
                .command = "ENTER DOOR",
                .required_flag       = FLAG_OPEN_DOOR,
                .flag_to_check       = 0,
                .message_if_unset    = message010a,
                .set_flag_if_unset   = 0,
                .next_scene_if_unset = 1,
                .message_if_set      = "",
                .set_flag_if_set     = 0,
                .next_scene_if_set   = 0,
            },
            {
                .command = ""
            }
        }
    },

    // ROOM 2
    {
        .id = 1,
        .flag_to_check       = 0,
        .next_scene_if_unset = 0,
        .next_scene_if_set   = 0,
        .graphic_bank        = 2,
        .graphic_ptn0        = PTN_ROOM2_0,
        .graphic_ptn1        = PTN_ROOM2_1,
        .graphic_col0        = COL_ROOM2_0,
        .graphic_col1        = COL_ROOM2_1,
        .message             = message0200,
        .choices = {
            {
                .command = "LOOK DOOR",
                .required_flag       = 0,
                .flag_to_check       = 0,
                .message_if_unset    = message0201,
                .set_flag_if_unset   = 0,
                .next_scene_if_unset = 0,
                .message_if_set      = "",
                .set_flag_if_set     = 0,
                .next_scene_if_set   = 0,
            },
            {
                .command = "LOOK PILLER",
                .required_flag       = 0,
                .flag_to_check       = 0,
                .message_if_unset    = message0202,
                .set_flag_if_unset   = 0,
                .next_scene_if_unset = 0,
                .message_if_set      = "",
                .set_flag_if_set     = 0,
                .next_scene_if_set   = 0,
            },
            {
                .command = "POLISH PILLER",
                .required_flag       = 0,
                .flag_to_check       = FLAG_POLISHED,
                .message_if_unset    = message0203,
                .set_flag_if_unset   = FLAG_POLISHED,
                .next_scene_if_unset = 0,
                .message_if_set      = message0204,
                .set_flag_if_set     = 0,
                .next_scene_if_set   = 2,
            },
            {
                .command = ""
            }
        }
    },

    // ROOM 3
    {
        .id = 2,
        .flag_to_check       = 0,
        .next_scene_if_unset = 0,
        .next_scene_if_set   = 0,
        .graphic_bank        = 3,
        .graphic_ptn0        = PTN_ROOM3_0,
        .graphic_ptn1        = PTN_ROOM3_1,
        .graphic_col0        = COL_ROOM3_0,
        .graphic_col1        = COL_ROOM3_1,
        .message             = message0300,
        .choices = {
            {
                .command = "LOOK SKELTON",
                .required_flag       = 0,
                .flag_to_check       = 0,
                .message_if_unset    = message0301,
                .set_flag_if_unset   = 0,
                .next_scene_if_unset = 0,
                .message_if_set      = "",
                .set_flag_if_set     = 0,
                .next_scene_if_set   = 0,
            },
            {
                .command = "",
            }
        }
    },

    // ENDING
    {
        .id = 3,
        .flag_to_check       = 0,
        .next_scene_if_unset = 0,
        .next_scene_if_set   = 0,
        .graphic_ptn0        = 0,
        .graphic_ptn1        = 0,
        .graphic_col0        = 0,
        .graphic_col1        = 0,
        .message             = message0400,
        .choices = {
            {
                .command = "",
                .required_flag       = 0,
                .flag_to_check       = 0,
                .message_if_unset    = "",
                .set_flag_if_unset   = 0,
                .next_scene_if_unset = 0,
                .message_if_set      = "",
                .set_flag_if_set     = 0,
                .next_scene_if_set   = 0,
            }
        }
    }
};

// コマンド入力用バッファ
uint8_t input_buffer[32 - 9];

// キー入力バッファ
uint8_t key_buffer;

// フラグ管理（ビット演算で扱う）
uint8_t game_flags = 0;

// コマンド一致フラグ
uint8_t matched = 0;

// 展開先ワークエリア
//uint8_t temp[2048*2];
uint8_t temp[2048];

// シーンデータへの参照
Scene scene;

// 選択肢データへの参照
Choice *choice;


void input_command(uint8_t _y)
{
    // RETURNキー入力フラグ
    bool_t enter_flg = false;

    // 入力バッファインデックス
    uint8_t buffer_ix = 0;

    // バッファクリア
    for (uint8_t i = 0; i < sizeof(input_buffer); i++) {
        input_buffer[i] = 0x00;
    }
    buffer_ix = 0;

    // プロンプト表示
    put_message(0, _y, promptMessage);

    // ループ開始
    while(enter_flg == false) {
        // バッファ内容表示
        put_message(sizeof(promptMessage) - 1, _y , input_buffer);

        // キーバッファクリア
        msx_clearkey();

        // キー入力を待つ
//        key_buffer = getkeycode();    // TODO: ほんとはこっちにしたい(stdioを使わない)
        key_buffer = getkey();

        // RETURNキーならループ終了
        if (key_buffer == 0x0a) {
            if (buffer_ix > 0) {
                enter_flg = true;
            }
            continue;
        }
/*
        // ESCキーならバッファクリア
        if (key_buffer == 0x1b) {
            clear_inputBuffer();
            continue;
        }
*/
        // DELキーでバッファがあればバッファから1文字削除
        if (key_buffer == 0x08) {
            if (buffer_ix > 0) {
                buffer_ix--;
                put_message(sizeof(promptMessage) + buffer_ix - 1, _y, blank);
            }
            input_buffer[buffer_ix] = 0x00;
            continue;
        }

        // カーソルキーは無視する
        if (key_buffer == 0x1c || key_buffer == 0x1d || key_buffer == 0x1e || key_buffer == 0x1f) {
            continue;
        }

        // 小文字のアルファベットは大文字とする
        if (key_buffer >= 'a' && key_buffer <= 'z') {
            key_buffer -= 0x20;
        }

        // それ以外ならバッファに追加（←ローマ字カナ入力をするときは、ここで変換処理を呼べばOK）
        if (buffer_ix < 31 - sizeof(promptMessage)) {
            input_buffer[buffer_ix++] = key_buffer;
        }
    }

    return;
}


// シーンの実行処理
void run_scene(int start_scene_id)
{
    uint8_t scene_id = start_scene_id;
    uint8_t previous_scene_id = 0xff;

    while (scene_id >= 0) {

        if (scene_id != previous_scene_id) {

            scene = scenes[scene_id];

            // シーン分岐のみ行う場合
            if (scene.flag_to_check) {
                if (game_flags & scene.flag_to_check) {
                    scene_id = scene.next_scene_if_set;
                } else {
                    scene_id = scene.next_scene_if_unset;
                }

            // 通常のシーンの場合
            } else {
                // 最初のシーン以外は、前シーンの最後のメッセージ表示後にキー入力待ちを行う
                if (scene_id != 0) {
                    put_message(0, 23, waitMessage);
                    keywait();
                    clear_message();
                }

                // グラフィックデータが設定されている場合はデータを展開し表示する
                if (scene.graphic_ptn0 != 0) {
                    switch_bank(scene.graphic_bank);
                    unpack(scene.graphic_ptn0, temp);
                    vdp_vwrite(temp, VRAM_PTN_GENR_TBL1, 0x04c0);
                    unpack(scene.graphic_ptn1, temp);
                    vdp_vwrite(temp, VRAM_PTN_GENR_TBL2, 0x0390);
                    unpack(scene.graphic_col0, temp);
                    vdp_vwrite(temp, VRAM_COLOR_TBL1, 0x04c0);
                    unpack(scene.graphic_col1, temp);
                    vdp_vwrite(temp, VRAM_COLOR_TBL2, 0x0390);
                }

                // メッセージを表示
                put_message(0, 17, scene.message);

                // 直前のシーンIDを現在のシーンIDに置き換える
                previous_scene_id = scene_id;
            }

        } else {
            // コマンド入力
            input_command(23);

            // メッセージ表示エリアクリア
            clear_message();

            // コマンド一致フラグOFF
            matched = 0;

            for (uint8_t i = 0; i < MAX_CHOICES; i++) {

                // シーンデータの選択肢データへの参照を取得
                choice = &scene.choices[i];

                // 選択肢データのコマンド未設定時はループ終了
                if (choice->command[0] == '\0') {
//                    put_message(31, 23, debug);
                    break;
                } 

                // 選択肢データのコマンド＝入力コマンド and
                // 条件フラグ＝設定なし or 条件フラグの対象＝ON の場合
                if ((strcmp(choice->command, input_buffer) == 0) &&
                    ((choice->required_flag == 0) || (game_flags & choice->required_flag))) {

                    // コマンド一致フラグON
                    matched = 1;

                    // 選択肢データのチェック対象フラグ＝設定あり and
                    // チェック対象フラグ＝ON の場合
                    if ((choice->flag_to_check) && (game_flags & choice->flag_to_check)) {
                        // メッセージ表示
                        put_message(0, 17, choice->message_if_set);
                        // フラグセット
                        if (choice->set_flag_if_set) {
                            game_flags |= choice->set_flag_if_set;
                        }
                        // シーン遷移
                        if (choice->next_scene_if_set) {
                            scene_id = choice->next_scene_if_set;
                        }

                    } else {
                        // メッセージ表示
                        put_message(0, 17, choice->message_if_unset);
                        // フラグセット
                        if (choice->set_flag_if_unset) {
                            game_flags |= choice->set_flag_if_unset;
                        }
                        // シーン遷移
                        if (choice->next_scene_if_unset) {
                            scene_id = choice->next_scene_if_unset;
                        }
                    }

                    break;
                }
            }

            if (!matched) {
                put_message(0, 17, dontMessage);
            }

        }
    }
}


void main()
{
    // 画面初期化
    set_color(15, 1, 1);
    set_mangled_mode();
    msx_set_sprite_mode(sprite_large);

    // キークリックスイッチOFF
    *(uint8_t *)MSX_CLIKSW = 0;

    // キーのオートリピート開始までの時間間隔
    // C-BIOSでは初期値1のため、明示的に設定
    *(uint8_t *)MSX_REPCNT = 50;

    // フォントパターンをバンク0～3のパターンジェネレータテーブル／カラーテーブルに設定
    vdp_vwrite(FONT_PTN_TBL, VRAM_PTN_GENR_TBL3, 0x0800);
    vdp_vwrite(FONT_COL_TBL, VRAM_COLOR_TBL3, 0x0800);

    // バンク1～2のパターンネームテーブル設定
    uint8_t code = 0;

    for (uint16_t i = 0; i < 256; i++) {
        temp[i] = 254;
    }
    code = 0;
    for (uint8_t i = 0; i < 8; i++) {
        for (uint8_t j = 0; j < 19; j++) {
            temp[i * 32 + j] = code++;
        }
    }
    vdp_vwrite(temp, VRAM_PTN_NAME_TBL1, 0x100);

    for (uint16_t i = 0; i < 256; i++) {
        temp[i] = 254;
    }
    code = 0;
    for (uint8_t i = 0; i < 6; i++) {
        for (uint8_t j = 0; j < 19; j++) {
            temp[i * 32 + j] = code++;
        }
    }
    vdp_vwrite(temp, VRAM_PTN_NAME_TBL2, 0x100);


    while (0 == 0) {
        run_scene(0);
    }
}
