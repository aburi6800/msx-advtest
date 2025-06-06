// License:MIT License
// copyright-holders:aburi6800 (Hitoshi Iwai)

#include "../include/resource.h"
#include "./message.c"
#include "./flag.c"


// シーン数
#define SCENE_NUM 23


// シーンIDの列挙型
typedef enum {
    NOSCENE,
    TITLE,
    PROLOGUE,
    DESCRIPTION,
    SCENE01,
    SCENE02,
    SCENE03,
    SCENE03B,
    SCENE03_1,
    SCENE03_2,
    SCENE04,
    SCENE04_1,
    SCENE04_2,
    SCENE05,
    SCENE05B,
    SCENE05_1,
    SCENE05_2,
    SCENE05_3,
    SCENE06,
    SCENE06_1,
    SCENE06_2,
    SCENE07,
    OVER,
    ENDING
} SceneId;


// 選択肢定義
typedef struct {
    uint16_t required_flag;         // この選択肢が表示・実行できる条件(0で条件なし)
    const char *commands[5];        // プレイヤーの入力コマンド(1選択肢につき最大5コマンド)
    uint16_t flag_to_check;         // メッセージの分岐に使うフラグ(0で分岐なし)
    uint8_t *message_if_unset;      // フラグが未設定のときの表示メッセージ
    uint16_t set_flag_if_unset;     // 実行後にセットするフラグ(フラグ未設定時)
    SceneId next_sceneId_if_unset;  // 次のシーン名(空で遷移なし)
    uint8_t *message_if_set;        // フラグが設定済のときの表示メッセージ
    uint16_t set_flag_if_set;       // 実行後にセットするフラグ(フラグ設定時)
    SceneId next_sceneId_if_set;    // 次のシーン名(空で遷移なし)
} Choice;


// シーン定義
typedef struct {
    SceneId sceneId;                // シーンID
    uint16_t flag_to_check;         // シーンの分岐に使うフラグ(0で分岐なし)
    SceneId next_sceneId_if_unset;  // フラグが未設定のときのシーンID
    SceneId next_sceneId_if_set;    // フラグが設定済のときのシーンID
    uint8_t graphic_bank;           // グラフィックデータのバンク
    uint8_t *graphic_ptn0;          // グラフィックデータ(パターンジェネレータテーブル PAGE0)
    uint8_t *graphic_ptn1;          // グラフィックデータ(パターンジェネレータテーブル PAGE1)
    uint8_t *graphic_col0;          // グラフィックデータ(カラーテーブル PAGE0)
    uint8_t *graphic_col1;          // グラフィックデータ(カラーテーブル PAGE1)
    uint8_t *message;               // シーンの最初に表示するメッセージ
    uint8_t *choices;
} Scene;


// シーンデータ
Choice choicesNull[1] = {
    {   
        .required_flag          = 0,
        .commands               = {NULL}
    }
};


Choice choicesTitle[2] = {
    {   
        .required_flag          = 0,
        .commands               = {"START", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
//        .next_sceneId_if_unset  = SCENE01,
        .next_sceneId_if_unset  = PROLOGUE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE
    },
    {   
        .required_flag          = 0,
        .commands               = {NULL}
    }
};

Scene sceneTitle = {
    .sceneId                = TITLE,
    .flag_to_check          = 0,
    .next_sceneId_if_unset  = NOSCENE,
    .next_sceneId_if_set    = NOSCENE,
    .graphic_bank           = 1,
    .graphic_ptn0           = TITLE_PTN_BLK0,
    .graphic_ptn1           = TITLE_PTN_BLK1,
    .graphic_col0           = TITLE_COL_BLK0,
    .graphic_col1           = TITLE_COL_BLK1,
    .message                = message00000,
    .choices                = choicesTitle
};


Choice choicesPrologue[3] = {
    {   
        .required_flag          = 0,
        .commands               = {"Y", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = DESCRIPTION,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE
    },
    {   
        .required_flag          = 0,
        .commands               = {"N", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE01,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE
    },
    {   
        .required_flag          = 0,
        .commands               = {NULL}
    }
};

Scene scenePrologue = {
    .sceneId                = PROLOGUE,
    .flag_to_check          = 0,
    .next_sceneId_if_unset  = NOSCENE,
    .next_sceneId_if_set    = NOSCENE,
    .graphic_bank           = 1,
    .graphic_ptn0           = PLLG_PTN_BLK0,
    .graphic_ptn1           = PLLG_PTN_BLK1,
    .graphic_col0           = PLLG_COL_BLK0,
    .graphic_col1           = PLLG_COL_BLK1,
    .message                = message97000,
    .choices                = choicesPrologue
};


Scene sceneDescription = {
    .sceneId                = DESCRIPTION,
    .flag_to_check          = 0,
    .next_sceneId_if_unset  = SCENE01,
    .next_sceneId_if_set    = NOSCENE,
    .graphic_bank           = 1,
    .graphic_ptn0           = NULL,
    .graphic_ptn1           = NULL,
    .graphic_col0           = NULL,
    .graphic_col1           = NULL,
    .message                = messagehelp,
    .choices                = choicesNull
};


Scene sceneOver = {
    .sceneId                = OVER,
    .flag_to_check          = 0,
    .next_sceneId_if_unset  = TITLE,
    .next_sceneId_if_set    = NOSCENE,
    .graphic_bank           = 1,
    .graphic_ptn0           = OVER_PTN_BLK0,
    .graphic_ptn1           = OVER_PTN_BLK1,
    .graphic_col0           = OVER_COL_BLK0,
    .graphic_col1           = OVER_COL_BLK1,
    .message                = message99000,
    .choices                = choicesNull
};


Scene sceneEnding = {
    .sceneId                = ENDING,
    .flag_to_check          = 0,
    .next_sceneId_if_unset  = TITLE,
    .next_sceneId_if_set    = NOSCENE,
    .graphic_bank           = 1,
    .graphic_ptn0           = ENDING_PTN_BLK0,
    .graphic_ptn1           = ENDING_PTN_BLK1,
    .graphic_col0           = ENDING_COL_BLK0,
    .graphic_col1           = ENDING_COL_BLK1,
    .message                = message98000,
    .choices                = choicesNull
};


Choice choices010[8] = {
    {
        .required_flag          = 0,
        .commands               = {"LOOK ROOM", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = message01001,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK TABLE", NULL},
        .flag_to_check          = FLAG_HAVE_KEY,
        .message_if_unset       = message01002,
        .set_flag_if_unset      = FLAG_FOUND_KEY,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = message01003,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE
    },
    {
        .required_flag          = FLAG_FOUND_KEY,
        .commands               = {"LOOK KEY", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = message01011,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE
    },
    {
        .required_flag          = FLAG_FOUND_KEY,
        .commands               = {"GET KEY", "TAKE KEY", NULL},
        .flag_to_check          = FLAG_HAVE_KEY,
        .message_if_unset       = message01004,
        .set_flag_if_unset      = FLAG_HAVE_KEY,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = message01005,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK DOOR", NULL},
        .flag_to_check          = FLAG_OPEN_DOOR,
        .message_if_unset       = message01006,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = message01007,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE
    },
    {
        .required_flag          = 0,
        .commands               = {"OPEN DOOR", "USE KEY", NULL},
        .flag_to_check          = FLAG_HAVE_KEY,
        .message_if_unset       = message01008,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = message01009,
        .set_flag_if_set        = FLAG_OPEN_DOOR,
        .next_sceneId_if_set    = NOSCENE
    },
    {
        .required_flag          = FLAG_OPEN_DOOR,
        .commands               = {"ENTER DOOR", "GO FORWARD", "FORWARD", "F", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = message01010,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE02,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .commands               = {NULL}
    }
};

Scene scene010 = {
    .sceneId                = SCENE01,
    .flag_to_check          = 0,
    .next_sceneId_if_unset  = NOSCENE,
    .next_sceneId_if_set    = NOSCENE,
    .graphic_bank           = 2,
    .graphic_ptn0           = SC010_PTN_BLK0,
    .graphic_ptn1           = SC010_PTN_BLK1,
    .graphic_col0           = SC010_COL_BLK0,
    .graphic_col1           = SC010_COL_BLK1,
    .message                = message01000,
    .choices                = choices010
};


Choice choices020[6] = {
    {
        .required_flag          = 0,
        .commands               = {"GO BACK", "BACK", "B", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE01,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"GO LEFT", "LEFT", "L", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE06,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"GO RIGHT", "RIGHT", "R", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE03,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"GO FORWARD", "FORWARD", "F", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE04,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK ROOM", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = message02001,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .commands               = {NULL}
    }
};

Scene scene020 = {
    .sceneId                = SCENE02,
    .flag_to_check          = 0,
    .next_sceneId_if_unset  = NOSCENE,
    .next_sceneId_if_set    = NOSCENE,
    .graphic_bank           = 2,
    .graphic_ptn0           = SC020_PTN_BLK0,
    .graphic_ptn1           = SC020_PTN_BLK1,
    .graphic_col0           = SC020_COL_BLK0,
    .graphic_col1           = SC020_COL_BLK1,
    .message                = message02000,
    .choices                = choices020
};


Scene scene030 = {
    .sceneId                = SCENE03,
    .flag_to_check          = FLAG_FAIL_APPLE,
    .next_sceneId_if_unset  = SCENE03_1,
    .next_sceneId_if_set    = SCENE03B,
    .graphic_bank           = 0,
    .graphic_ptn0           = NULL,
    .graphic_ptn1           = NULL,
    .graphic_col0           = NULL,
    .graphic_col1           = NULL,
    .message                = NULL,
    .choices                = choicesNull
};


Scene scene03B = {
    .sceneId                = SCENE03B,
    .flag_to_check          = FLAG_HAVE_APPLE,
    .next_sceneId_if_unset  = SCENE03_2,
    .next_sceneId_if_set    = SCENE03_1,
    .graphic_bank           = 0,
    .graphic_ptn0           = NULL,
    .graphic_ptn1           = NULL,
    .graphic_col0           = NULL,
    .graphic_col1           = NULL,
    .message                = NULL,
    .choices                = choicesNull
};


Choice choices031[7] = {
    {
        .required_flag          = 0,
        .commands               = {"GO BACK", "BACK", "B", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE02,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK TREE", "LOOK ROOM", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = message03101,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"CLIMB TREE", "GO UP", "UP", "U", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = message03102,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = OVER,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"KICK TREE", "SWING TREE", "PUSH TREE", NULL},
        .flag_to_check          = FLAG_HAVE_APPLE,
        .message_if_unset       = message03103,
        .set_flag_if_unset      = FLAG_FAIL_APPLE,
        .next_sceneId_if_unset  = SCENE03_2,
        .message_if_set         = message03104,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = FLAG_HAVE_APPLE,
        .commands               = {"EAT APPLE", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = message03105,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = OVER,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = FLAG_HAVE_APPLE,
        .commands               = {"LOOK APPLE", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = message03202,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .commands = {NULL}
    }
};

Scene scene031 = {
    .sceneId                = SCENE03_1,
    .flag_to_check          = 0,
    .next_sceneId_if_unset  = NULL,
    .next_sceneId_if_set    = NULL,
    .graphic_bank           = 2,
    .graphic_ptn0           = SC031_PTN_BLK0,
    .graphic_ptn1           = SC031_PTN_BLK1,
    .graphic_col0           = SC031_COL_BLK0,
    .graphic_col1           = SC031_COL_BLK1,
    .message                = message03100,
    .choices                = choices031
};


Choice choices032[8] = {
    {
        .required_flag          = 0,
        .commands               = {"GO BACK", "BACK", "B", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE02,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NULL,
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK TREE", "LOOK ROOM", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = message03201,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"CLIMB TREE", "GO UP", "UP", "U", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = message03202,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = OVER,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"KICK TREE", "SWING TREE", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = message03104,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK APPLE", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = message03202,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"GET APPLE", "TAKE APPLE", NULL},
        .flag_to_check          = FLAG_HAVE_APPLE,
        .message_if_unset       = message03203,
        .set_flag_if_unset      = FLAG_HAVE_APPLE,
        .next_sceneId_if_unset  = SCENE03_1,
        .message_if_set         = message03204,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"EAT APPLE", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = message03205,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .commands               = {NULL}
    }
};

Scene scene032 = {
    .sceneId                = SCENE03_2,
    .flag_to_check          = 0,
    .next_sceneId_if_unset  = NOSCENE,
    .next_sceneId_if_set    = NOSCENE,
    .graphic_bank           = 2,
    .graphic_ptn0           = SC032_PTN_BLK0,
    .graphic_ptn1           = SC032_PTN_BLK1,
    .graphic_col0           = SC032_COL_BLK0,
    .graphic_col1           = SC032_COL_BLK1,
    .message                = message03200,
    .choices                = choices032
};


Scene scene040 = {
    .sceneId                = SCENE04,
    .flag_to_check          = FLAG_KILL_SNAKE,
    .next_sceneId_if_unset  = SCENE04_1,
    .next_sceneId_if_set    = SCENE04_2,
    .graphic_bank           = 0,
    .graphic_ptn0           = NULL,
    .graphic_ptn1           = NULL,
    .graphic_col0           = NULL,
    .graphic_col1           = NULL,
    .message                = NULL,
    .choices                = choicesNull
};


Choice choices041[6] = {
    {
        .required_flag          = 0,
        .commands               = {"GO BACK", "BACK", "B", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE02,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"GO FORWARD", "FORWARD", "F", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = message04101,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = OVER,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK SNAKE", NULL},
        .flag_to_check          = FLAG_LOOK_SNAKE,
        .message_if_unset       = message04102,
        .set_flag_if_unset      = FLAG_LOOK_SNAKE,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = message04101,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = OVER,
    },
    {
        .required_flag          = FLAG_HAVE_APPLE,
        .commands               = {"THROW APPLE", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = message04103,
        .set_flag_if_unset      = FLAG_KILL_SNAKE,
        .next_sceneId_if_unset  = SCENE04,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = SCENE04,
    },
    {
        .required_flag          = 0,
        .commands               = {"EAT APPLE", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = message03205,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = OVER,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .commands = {NULL}
    }
};

Scene scene041 = {
    .sceneId                = SCENE04_1,
    .flag_to_check          = 0,
    .next_sceneId_if_unset  = NULL,
    .next_sceneId_if_set    = NULL,
    .graphic_bank           = 2,
    .graphic_ptn0           = SC041_PTN_BLK0,
    .graphic_ptn1           = SC041_PTN_BLK1,
    .graphic_col0           = SC041_COL_BLK0,
    .graphic_col1           = SC041_COL_BLK1,
    .message                = message04100,
    .choices                = choices041
};


Choice choices042[5] = {
    {
        .required_flag          = 0,
        .commands               = {"GO BACK", "BACK", "B", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE02,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"GO FORWARD", "FORWARD", "F", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE05,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK SNAKE", "LOOK BORN", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = message04201,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"GET BORN", "TAKE BORN", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = message04202,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = SCENE04,
    },
    {
        .commands = {NULL}
    }
};

Scene scene042 = {
    .sceneId                = SCENE04_2,
    .flag_to_check          = 0,
    .next_sceneId_if_unset  = NULL,
    .next_sceneId_if_set    = NULL,
    .graphic_bank           = 2,
    .graphic_ptn0           = SC042_PTN_BLK0,
    .graphic_ptn1           = SC042_PTN_BLK1,
    .graphic_col0           = SC042_COL_BLK0,
    .graphic_col1           = SC042_COL_BLK1,
    .message                = message04200,
    .choices                = choices042
};


Scene scene050 = {
    .sceneId                = SCENE05,
    .flag_to_check          = FLAG_HAVE_SWORD,
    .next_sceneId_if_unset  = SCENE05B,
    .next_sceneId_if_set    = SCENE05_2,
    .graphic_bank           = 0,
    .graphic_ptn0           = NULL,
    .graphic_ptn1           = NULL,
    .graphic_col0           = NULL,
    .graphic_col1           = NULL,
    .message                = NULL,
    .choices                = choicesNull
};


Scene scene05B = {
    .sceneId                = SCENE05B,
    .flag_to_check          = FLAG_BROKEN_SWORD,
    .next_sceneId_if_unset  = SCENE05_1,
    .next_sceneId_if_set    = SCENE05_3,
    .graphic_bank           = 0,
    .graphic_ptn0           = NULL,
    .graphic_ptn1           = NULL,
    .graphic_col0           = NULL,
    .graphic_col1           = NULL,
    .message                = NULL,
    .choices                = choicesNull
};


Choice choices051[7] = {
    {
        .required_flag          = 0,
        .commands               = {"GO BACK", "BACK", "B", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE04,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK ROCK", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = message05101,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK SWORD", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = message05102,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"PULL SWORD", NULL},
        .flag_to_check          = FLAG_PULL_SWORD,
        .message_if_unset       = message05103,
        .set_flag_if_unset      = FLAG_PULL_SWORD,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = message05104,
        .set_flag_if_set        = FLAG_BROKEN_SWORD,
        .next_sceneId_if_set    = SCENE05_3,
    },
    {
        .required_flag          = 0,
        .commands               = {"PUSH SWORD", NULL},
        .flag_to_check          = FLAG_PULL_SWORD,
        .message_if_unset       = message05105,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = message05106,
        .set_flag_if_set        = FLAG_HAVE_SWORD,
        .next_sceneId_if_set    = SCENE05_2,
    },
    {
        .required_flag          = 0,
        .commands               = {"MOVE ROCK", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = message05107,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .commands = {NULL}
    }
};

Scene scene051 = {
    .sceneId                = SCENE05_1,
    .flag_to_check          = 0,
    .next_sceneId_if_unset  = NULL,
    .next_sceneId_if_set    = NULL,
    .graphic_bank           = 3,
    .graphic_ptn0           = SC051_PTN_BLK0,
    .graphic_ptn1           = SC051_PTN_BLK1,
    .graphic_col0           = SC051_COL_BLK0,
    .graphic_col1           = SC051_COL_BLK1,
    .message                = message05100,
    .choices                = choices051
};


Choice choices052[4] = {
    {
        .required_flag          = 0,
        .commands               = {"GO BACK", "BACK", "B", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE04,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK ROCK", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = message05201,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"MOVE ROCK", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = message05107,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .commands = {NULL}
    }
};

Scene scene052 = {
    .sceneId                = SCENE05_2,
    .flag_to_check          = 0,
    .next_sceneId_if_unset  = NULL,
    .next_sceneId_if_set    = NULL,
    .graphic_bank           = 3,
    .graphic_ptn0           = SC052_PTN_BLK0,
    .graphic_ptn1           = SC052_PTN_BLK1,
    .graphic_col0           = SC052_COL_BLK0,
    .graphic_col1           = SC052_COL_BLK1,
    .message                = message05200,
    .choices                = choices052
};


Choice choices053[4] = {
    {
        .required_flag          = 0,
        .commands               = {"GO BACK", "BACK", "B", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE04,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK ROCK", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = message05301,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"MOVE ROCK", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = message05107,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .commands = {NULL}
    }
};

Scene scene053 = {
    .sceneId                = SCENE05_3,
    .flag_to_check          = 0,
    .next_sceneId_if_unset  = NULL,
    .next_sceneId_if_set    = NULL,
    .graphic_bank           = 3,
    .graphic_ptn0           = SC053_PTN_BLK0,
    .graphic_ptn1           = SC053_PTN_BLK1,
    .graphic_col0           = SC053_COL_BLK0,
    .graphic_col1           = SC053_COL_BLK1,
    .message                = message05300,
    .choices                = choices053
};


Scene scene060 = {
    .sceneId                = SCENE06,
    .flag_to_check          = FLAG_KILL_SKELTON,
    .next_sceneId_if_unset  = SCENE06_1,
    .next_sceneId_if_set    = SCENE06_2,
    .graphic_bank           = 0,
    .graphic_ptn0           = NULL,
    .graphic_ptn1           = NULL,
    .graphic_col0           = NULL,
    .graphic_col1           = NULL,
    .message                = NULL,
    .choices                = choicesNull
};


Choice choices061[8] = {
    {
        .required_flag          = 0,
        .commands               = {"GO BACK", "BACK", "B", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE02,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK SKELTON", NULL},
        .flag_to_check          = FLAG_LOOK_SKELTON,
        .message_if_unset       = message06101,
        .set_flag_if_unset      = FLAG_LOOK_SKELTON,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = message06102,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = OVER,
    },
    {
        .required_flag          = FLAG_HAVE_SWORD,
        .commands               = {"USE SWORD", "EQUIP SWORD", NULL},
        .flag_to_check          = FLAG_EQUIP_SWORD,
        .message_if_unset       = message06103,
        .set_flag_if_unset      = FLAG_EQUIP_SWORD,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = message06104,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = FLAG_EQUIP_SWORD,
        .commands               = {"ATTACK SKELTON", "BATTLE SKELTON", "KILL SKELTON", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = message06105,
        .set_flag_if_unset      = FLAG_KILL_SKELTON,
        .next_sceneId_if_unset  = SCENE06_2,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"ATTACK SKELTON", "BATTLE SKELTON", "KILL SKELTON", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = message06106,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = OVER,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK SKARF", NULL},
        .flag_to_check          = FLAG_LOOK_SKELTON,
        .message_if_unset       = message06107,
        .set_flag_if_unset      = FLAG_LOOK_SKELTON,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = message06102,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = OVER,
    },
    {
        .required_flag          = 0,
        .commands               = {"GET SKARF", "TAKE SKARF", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = message06108,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .commands = {NULL}
    }
};

Scene scene061 = {
    .sceneId                = SCENE06_1,
    .flag_to_check          = 0,
    .next_sceneId_if_unset  = NULL,
    .next_sceneId_if_set    = NULL,
    .graphic_bank           = 3,
    .graphic_ptn0           = SC061_PTN_BLK0,
    .graphic_ptn1           = SC061_PTN_BLK1,
    .graphic_col0           = SC061_COL_BLK0,
    .graphic_col1           = SC061_COL_BLK1,
    .message                = message06100,
    .choices                = choices061
};


Choice choices062[8] = {
    {
        .required_flag          = 0,
        .commands               = {"GO BACK", "BACK", "B", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE02,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"GO FORWARD", "FORWARD", "F", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE07,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK SKELTON", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = message06201,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK SWORD", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = message06202,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"GET SWORD", "TAKE SWORD", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = message06203,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK SKARF", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = message06204,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"GET SKARF", "TAKE SKARF", NULL},
        .flag_to_check          = FLAG_HAVE_SKARF,
        .message_if_unset       = message06205,
        .set_flag_if_unset      = FLAG_HAVE_SKARF,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = message03204,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .commands = {NULL}
    }
};

Scene scene062 = {
    .sceneId                = SCENE06_2,
    .flag_to_check          = 0,
    .next_sceneId_if_unset  = NULL,
    .next_sceneId_if_set    = NULL,
    .graphic_bank           = 3,
    .graphic_ptn0           = SC062_PTN_BLK0,
    .graphic_ptn1           = SC062_PTN_BLK1,
    .graphic_col0           = SC062_COL_BLK0,
    .graphic_col1           = SC062_COL_BLK1,
    .message                = message06200,
    .choices                = choices062
};


Choice choices070[6] = {
    {
        .required_flag          = 0,
        .commands               = {"GO BACK", "BACK", "B", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE06,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK PILLER", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = message07001,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = FLAG_HAVE_SKARF,
        .commands               = {"POLISH PILLER", NULL},
        .flag_to_check          = FLAG_POLISH_PILLER,
        .message_if_unset       = message07002,
        .set_flag_if_unset      = FLAG_POLISH_PILLER,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = message07003,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = ENDING,
    },
    {
        .required_flag          = 0,
        .commands               = {"POLISH PILLER", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = message07004,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK ROOM", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = message07005,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .commands = {NULL}
    }
};

Scene scene070 = {
    .sceneId                = SCENE07,
    .flag_to_check          = 0,
    .next_sceneId_if_unset  = NULL,
    .next_sceneId_if_set    = NULL,
    .graphic_bank           = 3,
    .graphic_ptn0           = SC070_PTN_BLK0,
    .graphic_ptn1           = SC070_PTN_BLK1,
    .graphic_col0           = SC070_COL_BLK0,
    .graphic_col1           = SC070_COL_BLK1,
    .message                = message07000,
    .choices                = choices070
};


// シーンデータへの参照配列
Scene *scenes[SCENE_NUM] = {
    sceneTitle,
    sceneOver,
    sceneEnding,
    scenePrologue,
    sceneDescription,
    scene010,
    scene020,
    scene030,
    scene03B,
    scene031,
    scene032,
    scene040,
    scene041,
    scene042,
    scene050,
    scene05B,
    scene051,
    scene052,
    scene053,
    scene060,
    scene061,
    scene062,
    scene070
};
