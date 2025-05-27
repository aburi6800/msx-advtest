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
    const char *commands[5];        // プレイヤーの入力コマンド
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
                .commands = {"GO RIGHT", "RIGHT", "R", NULL},
                .required_flag       = 0,
                .flag_to_check       = 0,
                .message_if_unset    = "",
                .set_flag_if_unset   = 0,
                .next_scene_if_unset = 3,
                .message_if_set      = "",
                .set_flag_if_set     = 0,
                .next_scene_if_set   = 0,
            },
            {
                .commands = {NULL}
            }
        }
    },

    // SCENE 3
    {
        .id = 3,
        .flag_to_check       = FLAG_FAIL_APPLE,
        .next_scene_if_unset = 4,
        .next_scene_if_set   = 5,
        .graphic_bank        = 0,
        .graphic_ptn0        = NULL,
        .graphic_ptn1        = NULL,
        .graphic_col0        = NULL,
        .graphic_col1        = NULL,
        .message             = NULL,
        .choices = {
            {
                .commands = {NULL}
            }
        }
    },

    // SCENE 3-1
    {
        .id = 4,
        .flag_to_check       = 0,
        .next_scene_if_unset = 0,
        .next_scene_if_set   = 0,
        .graphic_bank        = 1,
        .graphic_ptn0        = SC04_PTN_BLK0,
        .graphic_ptn1        = SC04_PTN_BLK1,
        .graphic_col0        = SC04_COL_BLK0,
        .graphic_col1        = SC04_COL_BLK1,
        .message             = message0400,
        .choices = {
            {
                .commands = {"GO BACK", "BACK", "B", NULL},
                .required_flag       = 0,
                .flag_to_check       = 0,
                .message_if_unset    = "",
                .set_flag_if_unset   = 0,
                .next_scene_if_unset = 2,
                .message_if_set      = "",
                .set_flag_if_set     = 0,
                .next_scene_if_set   = 0,
            },
            {
                .commands = {"LOOK TREE", "LOOK ROOM", NULL},
                .required_flag       = 0,
                .flag_to_check       = 0,
                .message_if_unset    = message0401,
                .set_flag_if_unset   = 0,
                .next_scene_if_unset = 0,
                .message_if_set      = "",
                .set_flag_if_set     = 0,
                .next_scene_if_set   = 0,
            },
            {
                .commands = {"CLIMB TREE", "GO UP", "UP", "U", NULL},
                .required_flag       = 0,
                .flag_to_check       = 0,
                .message_if_unset    = message0402,
                .set_flag_if_unset   = 0,
                .next_scene_if_unset = 0,
                .message_if_set      = "",
                .set_flag_if_set     = 0,
                .next_scene_if_set   = 0,
            },
            {
                .commands = {"KICK TREE", "SWING TREE", NULL},
                .required_flag       = 0,
                .flag_to_check       = FLAG_HAVE_APPLE,
                .message_if_unset    = message0403,
                .set_flag_if_unset   = FLAG_FAIL_APPLE,
                .next_scene_if_unset = 5,
                .message_if_set      = message0404,
                .set_flag_if_set     = 0,
                .next_scene_if_set   = 0,
            },
            {
                .commands = {"EAT APPLE", NULL},
                .required_flag       = FLAG_HAVE_APPLE,
                .flag_to_check       = 0,
                .message_if_unset    = message0405,
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
    },

    // SCENE 3-2
    {
        .id = 5,
        .flag_to_check       = 0,
        .next_scene_if_unset = 0,
        .next_scene_if_set   = 0,
        .graphic_bank        = 1,
        .graphic_ptn0        = SC05_PTN_BLK0,
        .graphic_ptn1        = SC05_PTN_BLK1,
        .graphic_col0        = SC05_COL_BLK0,
        .graphic_col1        = SC05_COL_BLK1,
        .message             = message0500,
        .choices = {
            {
                .commands = {"GO BACK", "BACK", "B", NULL},
                .required_flag       = 0,
                .flag_to_check       = 0,
                .message_if_unset    = "",
                .set_flag_if_unset   = 0,
                .next_scene_if_unset = 2,
                .message_if_set      = "",
                .set_flag_if_set     = 0,
                .next_scene_if_set   = 0,
            },
            {
                .commands = {"LOOK TREE", "LOOK ROOM", NULL},
                .required_flag       = 0,
                .flag_to_check       = 0,
                .message_if_unset    = message0501,
                .set_flag_if_unset   = 0,
                .next_scene_if_unset = 0,
                .message_if_set      = "",
                .set_flag_if_set     = 0,
                .next_scene_if_set   = 0,
            },
            {
                .commands = {"CLIMB TREE", "GO UP", "UP", "U", NULL},
                .required_flag       = 0,
                .flag_to_check       = 0,
                .message_if_unset    = message0402,
                .set_flag_if_unset   = 0,
                .next_scene_if_unset = 0,
                .message_if_set      = "",
                .set_flag_if_set     = 0,
                .next_scene_if_set   = 0,
            },
            {
                .commands = {"KICK TREE", "SWING TREE", NULL},
                .required_flag       = 0,
                .flag_to_check       = 0,
                .message_if_unset    = message0404,
                .set_flag_if_unset   = 0,
                .next_scene_if_unset = 0,
                .message_if_set      = "",
                .set_flag_if_set     = 0,
                .next_scene_if_set   = 0,
            },
            {
                .commands = {"LOOK APPLE", NULL},
                .required_flag       = 0,
                .flag_to_check       = 0,
                .message_if_unset    = message0502,
                .set_flag_if_unset   = 0,
                .next_scene_if_unset = 0,
                .message_if_set      = "",
                .set_flag_if_set     = 0,
                .next_scene_if_set   = 0,
            },
            {
                .commands = {"GET APPLE", "TAKE APPLE", NULL},
                .required_flag       = 0,
                .flag_to_check       = FLAG_HAVE_APPLE,
                .message_if_unset    = message0503,
                .set_flag_if_unset   = FLAG_HAVE_APPLE,
                .next_scene_if_unset = 4,
                .message_if_set      = message0504,
                .set_flag_if_set     = 0,
                .next_scene_if_set   = 0,
            },
            {
                .commands = {"EAT APPLE", NULL},
                .required_flag       = 0,
                .flag_to_check       = 0,
                .message_if_unset    = message0505,
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
