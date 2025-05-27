// License:MIT License
// copyright-holders:aburi6800 (Hitoshi Iwai)

#include "./message.c"
#include "./flag.c"


// 最大シーン数
#define MAX_SCENES 10

// 最大選択肢数
#define MAX_CHOICES 16


// 選択肢定義
typedef struct {
    const char *commands[4];        // プレイヤーの入力コマンド
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
    uint8_t *graphic_ptn0;          // グラフィックデータ(パターンジェネレータテーブル PAGE0)
    uint8_t *graphic_ptn1;          // グラフィックデータ(パターンジェネレータテーブル PAGE1)
    uint8_t *graphic_col0;          // グラフィックデータ(カラーテーブル PAGE0)
    uint8_t *graphic_col1;          // グラフィックデータ(カラーテーブル PAGE1)
    uint8_t *message;               // シーンの最初に表示するメッセージ
    Choice choices[MAX_CHOICES];
} Scene;


// シーンデータ
Scene scenes[MAX_SCENES] = {
    // TITLE
    {
        .id = 0,
        .flag_to_check       = 0,
        .next_scene_if_unset = 0,
        .next_scene_if_set   = 0,
        .graphic_bank        = 1,
        .graphic_ptn0        = SC00_PTN_BLK0,
        .graphic_ptn1        = SC00_PTN_BLK1,
        .graphic_col0        = SC00_COL_BLK0,
        .graphic_col1        = SC00_COL_BLK1,
        .message             = message0000,
        .choices = {
            {
                .commands = {"START", NULL},
                .required_flag       = 0,
                .flag_to_check       = 0,
                .message_if_unset    = message0001,
                .set_flag_if_unset   = 0,
                .next_scene_if_unset = 1,
                .message_if_set      = "",
                .set_flag_if_set     = 0,
                .next_scene_if_set   = 0
            },
            {
                .commands = {NULL}
            }
        }
    },
    // SCENE 1
    {
        .id = 1,
        .flag_to_check       = 0,
        .next_scene_if_unset = 0,
        .next_scene_if_set   = 0,
        .graphic_bank        = 1,
        .graphic_ptn0        = SC01_PTN_BLK0,
        .graphic_ptn1        = SC01_PTN_BLK1,
        .graphic_col0        = SC01_COL_BLK0,
        .graphic_col1        = SC01_COL_BLK1,
        .message             = message0100,
        .choices = {
            {
                .commands = {"LOOK ROOM", NULL},
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
                .commands = {"LOOK TABLE", NULL},
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
                .commands = {"LOOK KEY", NULL},
                .required_flag       = FLAG_FOUND_KEY,
                .flag_to_check       = 0,
                .message_if_unset    = message0111,
                .set_flag_if_unset   = 0,
                .next_scene_if_unset = 0,
                .message_if_set      = "",
                .set_flag_if_set     = 0,
                .next_scene_if_set   = 0
            },
            {
                .commands = {"GET KEY", "TAKE KEY", NULL},
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
                .commands = {"LOOK DOOR", NULL},
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
                .commands = {"OPEN DOOR", NULL},
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
                .commands = {"ENTER DOOR", "GO FORWARD", "F", NULL},
                .required_flag       = FLAG_OPEN_DOOR,
                .flag_to_check       = 0,
                .message_if_unset    = message0110,
                .set_flag_if_unset   = 0,
                .next_scene_if_unset = 2,
                .message_if_set      = "",
                .set_flag_if_set     = 0,
                .next_scene_if_set   = 0,
            },
            {
                .commands = {NULL}
            }
        }
    },

    // SCENE 2
    {
        .id = 2,
        .flag_to_check       = 0,
        .next_scene_if_unset = 0,
        .next_scene_if_set   = 0,
        .graphic_bank        = 1,
        .graphic_ptn0        = SC02_PTN_BLK0,
        .graphic_ptn1        = SC02_PTN_BLK1,
        .graphic_col0        = SC02_COL_BLK0,
        .graphic_col1        = SC02_COL_BLK1,
        .message             = message0200,
        .choices = {
            {
                .commands = {"LOOK ROOM", NULL},
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
                .commands = {NULL}
            }
        }
    }
};
