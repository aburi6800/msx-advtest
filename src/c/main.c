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
#include "../include/music.h"
#include "../include/const.h"
#include "../include/unpack.h"
#include "../include/asmsub.h"
#include "../include/psgdriver.h"
#include "./scene.c"


// font.asmへの参照
extern uint8_t FONT_COL_TBL[];
extern uint8_t FONT_PTN_TBL[];

// プロンプトメッセージ
uint8_t promptMessage[] = { 0x3E, 0x00 };

// カーソル文字
uint8_t cursor[] = {0x5f, 0x00};

// ヘルプコマンド
uint8_t helpCommand[] = {0x48, 0x45, 0x4C, 0x50, 0x00};

// ブランク文字
uint8_t *blank = " ";

// デバッグ用文字
uint8_t *debug = "*";

// コマンド入力用バッファ
uint8_t input_buffer[32 - sizeof(promptMessage)];

// キー入力バッファ
uint8_t key_buffer;

// フラグ管理（ビット演算で扱う）
uint16_t game_flags = 0;

// 展開先ワークエリア
uint8_t temp[2048];

// シーンデータへの参照
Scene *scene;

// 選択肢データ
Choice choice;


void input_command()
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
    put_message_direct(0, PROMPT_LINE, promptMessage);

    // キーバッファクリア
    buffer_check();

    // ループ開始
    while(enter_flg == false) {
        // バッファ内容表示
        put_message_direct(sizeof(promptMessage) - 1, PROMPT_LINE , input_buffer);
        put_message_direct(sizeof(promptMessage) + buffer_ix - 1, PROMPT_LINE , cursor);

        // キーバッファクリア
//        msx_clearkey();

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
                put_message_direct(sizeof(promptMessage) + buffer_ix, PROMPT_LINE, blank);
            }
            input_buffer[buffer_ix] = 0x00;
            continue;
        }

        // カーソルキーは無視する
        if (key_buffer >= 0x1c && key_buffer <= 0x1f) {
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


// シーン列挙型からシーンの配列インデックスを取得
uint8_t getSceneIdx(SceneId sceneId)
{
    uint8_t idx = 0;

    for (uint8_t i = 0; i < SCENE_NUM; i++) {
        if (scenes[i]->sceneId == sceneId) {
            idx = i;
            break;
        }
    }

    return idx;
}


// シーンの実行処理
void run_scene(SceneId start_scene_id)
{
    // 現在のシーンの配列インデックス
    uint8_t scene_idx = getSceneIdx(start_scene_id);
    // 直前のシーンの配列インデックス
    uint8_t previous_scene_idx = 0xff;
    // ループ終了判定フラグ
    bool_t end_flg = false;
    // コマンド一致フラグ
    bool_t matched = false;

    while (!end_flg) {

        // 直前とシーンが変わっているか
        if (scene_idx != previous_scene_idx) {

            // 変わっている場合は、シーンを変更する
            scene = scenes[scene_idx];

            // フラグによるシーン分岐のみ行う場合
            if (scene->flag_to_check) {
                if (game_flags & scene->flag_to_check) {
                    scene_idx = getSceneIdx(scene->next_sceneId_if_set);
                } else {
                    scene_idx = getSceneIdx(scene->next_sceneId_if_unset);
                }

            // 通常のシーンの場合
            } else {
                // 直前のシーンIDを現在のシーンIDに置き換える
                previous_scene_idx = scene_idx;

                if (scene->graphic_ptn0 != NULL) {
                    // グラフィックデータが設定されている場合
                    // ブロック1/2のカラーテーブル設定（ブランク）
                    for (uint16_t i = 0; i < 2048; i++) {
                        temp[i] = 0x00;
                    }
                    vdp_vwrite(temp, VRAM_COLOR_TBL1, VRAM_COLOR_TBL_SIZE);
                    vdp_vwrite(temp, VRAM_COLOR_TBL2, VRAM_COLOR_TBL_SIZE);
                    // データを展開し表示する
                    switch_bank(scene->graphic_bank);
                    unpack(scene->graphic_ptn0, temp);
                    vdp_vwrite(temp, VRAM_PTN_GENR_TBL1, 0x04c0);
                    unpack(scene->graphic_ptn1, temp);
                    vdp_vwrite(temp, VRAM_PTN_GENR_TBL2, 0x0390);
                    unpack(scene->graphic_col0, temp);
                    vdp_vwrite(temp, VRAM_COLOR_TBL1, 0x04c0);
                    unpack(scene->graphic_col1, temp);
                    vdp_vwrite(temp, VRAM_COLOR_TBL2, 0x0390);
                }

                // メッセージを表示
                clear_message();
                put_message(0, 17, scene->message);

                if (scene->sceneId == OVER) {
                    // ゲームオーバーの場合、処理を終了する
                    end_flg = true;
                }

                if (scene->next_sceneId_if_unset != NOSCENE) {
                    // 次シーンの設定のみが行われている場合、キー入力を待ち、シーンを変更する
                    keywait();
                    scene_idx = getSceneIdx(scene->next_sceneId_if_unset);
                }
            }

        } else {
            // コマンド入力
            input_command();

            // メッセージ表示エリアクリア
            clear_message();

            // コマンド一致フラグOFF
            matched = false;

            if ((strcmp(helpCommand, input_buffer) == 0) &&
                (scene->sceneId != TITLE) &&
                (scene->sceneId != PROLOGUE)) {

                put_message(0, 17, HELPMESSAGE);
                matched = true;

            } else {

                Choice *choices = scene->choices;
                uint8_t i = 0;

                while (choices[i] != NULL) {

                    // シーンデータの選択肢データへの参照を取得
                    choice = choices[i];

                    // 選択肢データのコマンド未設定時はループ終了
                    if (choice.commands[0] == NULL) {
                        break;
                    } 

                    // すべてのコマンド候補に対して比較
                    for (int j = 0; choice.commands[j] != NULL; j++) {

                        // 選択肢データのコマンド＝入力コマンド and
                        // 条件フラグ＝設定なし or 条件フラグの対象＝ON の場合
                        if ((strcmp(choice.commands[j], input_buffer) == 0) &&
                            ((choice.required_flag == 0) || (game_flags & choice.required_flag))) {

                            // コマンド一致フラグON
                            matched = true;

                            // 選択肢データのチェック対象フラグ＝設定あり and
                            // チェック対象フラグ＝ON の場合
                            if ((choice.flag_to_check) && (game_flags & choice.flag_to_check)) {
                                // メッセージ表示
                                if (choice.message_if_set != NULL) {
                                    put_message(0, 17, choice.message_if_set);
                                }
                                // フラグセット
                                if (choice.set_flag_if_set) {
                                    game_flags |= choice.set_flag_if_set;
                                }
                                // シーン遷移
                                if (choice.next_sceneId_if_set) {
                                    if (choice.message_if_set != NULL) {
                                        keywait();
                                    }
                                    scene_idx = getSceneIdx(choice.next_sceneId_if_set);
                                }

                            // 上記以外の場合
                            } else {
                                // メッセージ表示
                                if (choice.message_if_unset != NULL) {
                                    put_message(0, 17, choice.message_if_unset);
                                }
                                // フラグセット
                                if (choice.set_flag_if_unset) {
                                    game_flags |= choice.set_flag_if_unset;
                                }
                                // シーン遷移
                                if (choice.next_sceneId_if_unset) {
                                    if (choice.message_if_unset != NULL) {
                                        keywait();
                                    }
                                    scene_idx = getSceneIdx(choice.next_sceneId_if_unset);
                                }
                            }

                            break;
                        }
                    }

                    if (matched) {
                        break;
                    }

                    i++;
                }
                
            }

            // コマンドがマッチしたか
            if (!matched) {
                // コマンドがマッチしなかった場合は、固定のメッセージを表示する
                put_message(0, 17, DONTMESSAGE);
            }
        }
    }
}


void init()
{
    // サウンドドライバー初期化
    sounddrv_init();

    // 言語判定
    check_region();

    // 画面初期化
    set_color(15, 1, 1);
    set_mangled_mode();
    msx_set_sprite_mode(sprite_large);
    set_palette();

    // キークリックスイッチOFF
    *(uint8_t *)MSX_CLIKSW = 0;

    // キーのオートリピート開始までの時間間隔
    // C-BIOSでは初期値1のため、明示的に設定（でもすぐ上書きされて変更できない）
    *(uint8_t *)MSX_REPCNT = 50;

    // ブロック1/2のパターンジェネレータテーブル設定（ブランク）
    for (uint16_t i = 0; i < sizeof(temp); i++) {
        temp[i] = 0x00;
    }
    vdp_vwrite(temp, VRAM_PTN_GENR_TBL1, VRAM_PTN_GENR_TBL_SIZE);
    vdp_vwrite(temp, VRAM_PTN_GENR_TBL2, VRAM_PTN_GENR_TBL_SIZE);

    // ブロック3のパターンジェネレータテーブル／カラーテーブル設定（フォントパターン）
    switch_bank(1);
    unpack(FONT_PTN_TBL, temp);
    vdp_vwrite(temp, VRAM_PTN_GENR_TBL3, VRAM_PTN_GENR_TBL_SIZE);
    unpack(FONT_COL_TBL, temp);
    vdp_vwrite(temp, VRAM_COLOR_TBL3, VRAM_PTN_GENR_TBL_SIZE);


    // パターンネームテーブル初期化
    uint8_t code = 0;

    for (uint16_t i = 0; i < 256; i++) {
        temp[i] = 254;
    }
    for (uint8_t i = 0; i < 8; i++) {
        for (uint8_t j = 0; j < 19; j++) {
            temp[i * 32 + j + 6] = code++;
        }
    }
    // ブロック1
    vdp_vwrite(temp, VRAM_PTN_NAME_TBL1, 0x100);
    // ブロック2
    vdp_vwrite(temp, VRAM_PTN_NAME_TBL2, 0x100);
}

void main()
{
    init();

    while (0 == 0) {
        // フラグ初期化
        game_flags = 0;

        // ゲームスタート
        run_scene(TITLE);
    }
}
