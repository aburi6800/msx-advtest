// License:MIT License
// copyright-holders:aburi6800 (Hitoshi Iwai)
// Compiler:zcc (z88dk)
// Include path:{$Z88DK_HOME}/include/* , {$Z88DK_HOME}/include/**/*

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
#include "./scene.c"


// font.asmへの参照
extern uint8_t FONT_COL_TBL[];
extern uint8_t FONT_PTN_TBL[];


// コマンド入力用バッファ
uint8_t input_buffer[32 - 9];

// キー入力バッファ
uint8_t key_buffer;

// フラグ管理（ビット演算で扱う）
uint8_t game_flags = 0;

// コマンド一致フラグ
uint8_t matched = 0;

// 展開先ワークエリア
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
                if (choice->commands[0] == '\0') {
                    break;
                } 

                // すべてのコマンド候補に対して比較
                for (int j = 0; choice->commands[j] != NULL; j++) {

                    // 選択肢データのコマンド＝入力コマンド and
                    // 条件フラグ＝設定なし or 条件フラグの対象＝ON の場合
                    if ((strcmp(choice->commands[j], input_buffer) == 0) &&
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
