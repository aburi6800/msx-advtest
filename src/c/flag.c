// License:MIT License
// copyright-holders:aburi6800 (Hitoshi Iwai)

// フラグ用定義
/*
#define FLAG_FOUND_KEY      (1 << 0)    // 鍵を見つけた
#define FLAG_HAVE_KEY       (1 << 1)    // 鍵を手に入れた
#define FLAG_OPEN_DOOR      (1 << 2)    // ドアを開けた
#define FLAG_FAIL_APPLE     (1 << 3)    // リンゴが落ちた
#define FLAG_HAVE_APPLE     (1 << 4)    // リンゴを手に入れた
#define FLAG_LOOK_SNAKE     (1 << 5)    // ヘビを見た
#define FLAG_KILL_SNAKE     (1 << 6)    // ヘビを倒した
#define FLAG_HAVE_SWORD     (1 << 7)    // 剣を取った
#define FLAG_PULL_SWORD     (1 << 8)    // 剣を抜こうとした
#define FLAG_BROKEN_SWORD   (1 << 9)    // 剣を折った
#define FLAG_POLISHED       (1 << 10)   // 柱を磨いた
*/
#define FLAG_FOUND_KEY      0b0000000000000001    // 鍵を見つけた
#define FLAG_HAVE_KEY       0b0000000000000010    // 鍵を手に入れた
#define FLAG_OPEN_DOOR      0b0000000000000100    // ドアを開けた
#define FLAG_FAIL_APPLE     0b0000000000001000    // リンゴが落ちた
#define FLAG_HAVE_APPLE     0b0000000000010000    // リンゴを手に入れた
#define FLAG_LOOK_SNAKE     0b0000000000100000    // ヘビを見た
#define FLAG_KILL_SNAKE     0b0000000001000000    // ヘビを倒した
#define FLAG_HAVE_SWORD     0b0000000010000000    // 剣を取った
#define FLAG_PULL_SWORD     0b0000000100000000    // 剣を抜こうとした
#define FLAG_BROKEN_SWORD   0b0000001000000000    // 剣を折った
#define FLAG_POLISHED       0b0000010000000000   // 柱を磨いた

//#define FLAG_HAVE_CROTH  (1 << 3)  // 赤い布を手に入れた
//#define FLAG_HAVE_SWORD  (1 << 5)  // 剣を手に入れた
