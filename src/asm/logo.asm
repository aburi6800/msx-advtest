; license:MIT License
; copyright-holders:aburi6800 (Hitoshi Iwai)

PUBLIC _boot_logo

WRVDP       EQU 0x0007
WRTVRM      EQU 0x004d
FILVRM      EQU 0x0056
LDIRVM      EQU 0x005c
CHGCLR      EQU 0x0062
;CLRSPR      EQU 0x0069
INIT32      EQU 0x006f
SETGRP      EQU 0x007e
GTSTCK      EQU 0x00d5
GTTRIG      EQU 0x00d8
SNSMAT      EQU 0x0141

FORCLR      EQU 0xf3e9
BAKCLR      EQU 0xf3ea
BDRCLR      EQU 0xf3eb
JIFFY       EQU 0xfc9e

JAPANESE    EQU 0x00
ENGLISH     EQU 0x01
SPANISH     EQU 0x02
PORTUGESE   EQU 0x03

EXTERN  _REGION
EXTERN SOUNDDRV_INIT
EXTERN SOUNDDRV_EXEC
EXTERN SOUNDDRV_BGMPLAY
EXTERN SOUNDDRV_SFXPLAY
EXTERN SOUNDDRV_STOP
EXTERN SOUNDDRV_PAUSE
EXTERN SOUNDDRV_RESUME
EXTERN SOUNDDRV_STATE


SECTION code_user

_boot_logo:
BOOT_LOGO:
    ; set screen mode
    call    INIT32
    call    SETGRP

    ld      a, 0
    ld      (FORCLR), a
    ld      (BAKCLR), a
    ld      (BDRCLR), a
    call    CHGCLR

    ; clear color table
    xor     a
    ld      hl, 0x2000
    ld      bc, 0x1800
    call    FILVRM

    ; clear pattern generate table
    xor     a
    ld      hl, 0x0000
    ld      bc, 0x1800
    call    FILVRM

    ; clear pattern name table
    xor     a
    ld      hl, 0x1800
    ld      bc, 0x0300
    call    FILVRM

    ; clear sprite
    call    CLRSPR

    ; set sprite mode
    di
    ld      a, (WRVDP)
    inc     a
    ld      c, a                    ; set control register#1(vdp write) port
    ld      a, 0b01100011           ; SI=1(16x16), MAG=1(double size)
    nop
    out     (c), a
    nop
    ld      a, 0b10000001           ; mode register #1
    nop
    out     (c), a
    nop
    ei

    ; set sprtite pattern
    ld      hl, SPRITE_PATTERN      ; source address(ram)
    ld      de, 0x3800              ; dist address(vram)
    ld      bc, 384                 ; data length
    call    LDIRVM

    ; set sprite attribute tablle
    ld      hl, 0x1b00              ; sprite attribute table address
    ld      de, LOGO_ATTR_DATA      ; data address

    ld      b, 24
BOOT_LOGO_1:
    ld      a, (de)                 ; get attribute data
    call    WRTVRM
    inc     de
    inc     hl

    djnz    BOOT_LOGO_1

BOOT_LOGO_2:
    xor     a
    ld      (JIFFY), a              ; jiffy reset
    
    ld      hl, LOGO_SE
    call    SOUNDDRV_BGMPLAY

BOOT_LOGO_3:
;    ld      a, (_REGION)
;    cp      1
;    jr      z, BOOT_LOGO_4          ; skip when english

    ld      a, 7
    call    SNSMAT
    cp      0b10111111              ; SELECT key
    jr      z, BOOT_SELECT_LANG     ; change language

BOOT_LOGO_4:
    ld      a, (JIFFY)              ; jiffy load
    cp      0xf0                    ; 4 seconds
    jr      nz, BOOT_LOGO_3

    call    CLRSPR

    ret


BOOT_SELECT_LANG:
    ; change color
    ld      a, 5
    ld      hl, 0x1b00              ; sprite attribute table address
    ld      de, 0x0003
    add     hl, de
    call    WRTVRM
    ld      de, 0x0004
    add     hl, de
    call    WRTVRM
    add     hl, de
    call    WRTVRM
    add     hl, de
    call    WRTVRM

BOOT_SELECT_LANG_0:
    ; set pattern number from region code
    ld      hl, LANGUAGE_ATTR_DATA + 2
    ld      a, (_REGION)
    add     a, a
    add     a, a
    add     a, a
    add     a, 24
    ld      (hl), a
    ld      de, 4
    add     hl, de
    add     a, 4
    ld      (hl), a

    ; set sprite attribute tablle
    ld      hl, 0x1b00 + (4 * 6)    ; sprite attribute table address
    ld      de, LANGUAGE_ATTR_DATA
    ld      b, 8
BOOT_SELECT_LANG_1:
    ld      a, (de)                 ; get attribute data
    call    WRTVRM
    inc     de
    inc     hl
    djnz    BOOT_SELECT_LANG_1

BOOT_SELECT_LANG_2:
    ; スペースキー入力があったら抜ける
    xor     a                       ; keyboard
    call    GTTRIG
    or      a
    jr      z, BOOT_SELECT_LANG_3

    call    CLRSPR
    ret

BOOT_SELECT_LANG_3:
    ; カーソルキー入力までループ
    xor     a                       ; keyboard
    call    GTSTCK

    ; 未入力 ?
    or      a
    jr      z, BOOT_SELECT_LANG_2
    
    ; 右 ?
    cp      3
    jr      nz, BOOT_SELECT_LANG_4

    ; 言語コード <> 2 ?
    ld      a, (_REGION)
    cp      2
    jr      z, BOOT_SELECT_LANG_4

    ; 言語コード + 1
    inc     a
    ld      (_REGION), a
    jr      BOOT_SELECT_LANG_5

BOOT_SELECT_LANG_4:
    ; 左 ?
    cp      7
    jr      nz, BOOT_SELECT_LANG_2
    
    ; 言語コード <> 0 ?
    ld      a, (_REGION)
    or      a
    jr      z, BOOT_SELECT_LANG_2
    
    ; 言語コード - 1
    dec     a
    ld      (_REGION), a

BOOT_SELECT_LANG_5:
    ; カーソルキー離すまでループ
    xor     a                       ; keyboard
    call    GTSTCK
    or      a
    jr      nz, BOOT_SELECT_LANG_5

    jr      BOOT_SELECT_LANG_0


CLRSPR:
    ld      b, 32
    ld      hl, 0x1b00
CLRSPR_L0:
    ld      a, 208
    call    WRTVRM
    inc     hl

    xor     a
    call    WRTVRM
    inc     hl

    call    WRTVRM
    inc     hl

    call    WRTVRM
    inc     hl

    djnz    CLRSPR_L0

    ret


SECTION rodata_user
SPRITE_PATTERN:
    ; #0
    DB  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    DB  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01
    DB  0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x01, 0x01
    DB  0x01, 0x13, 0x1B, 0x3F, 0x7F, 0xFF, 0xFF, 0xFD

    ; #4
    DB  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80
    DB  0x80, 0xC0, 0xC2, 0xE2, 0xE7, 0xEF, 0xFF, 0xFF
    DB  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    DB  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0

    ; #8
    DB  0x03, 0x03, 0x03, 0x07, 0x07, 0x06, 0x06, 0x06
    DB  0x06, 0x06, 0x06, 0x06, 0x03, 0x01, 0x00, 0x00
    DB  0xFD, 0xFC, 0xFE, 0x80, 0x00, 0x30, 0x30, 0xFC
    DB  0xFC, 0x30, 0x30, 0x03, 0x07, 0xFF, 0xFF, 0x00

    ; #12
    DB  0xFF, 0xFF, 0x7F, 0x20, 0x00, 0x06, 0x06, 0x19
    DB  0x19, 0x06, 0x06, 0xE0, 0xF0, 0xFF, 0xFF, 0x00
    DB  0xC0, 0xE0, 0xE0, 0xF0, 0x70, 0x30, 0x30, 0xB0
    DB  0xB0, 0x30, 0x30, 0x30, 0x60, 0xC0, 0x80, 0x00

    ; #16
    DB  0x00, 0x00, 0x00, 0x00, 0x0E, 0x1B, 0x1B, 0x1F
    DB  0x1B, 0x00, 0x1E, 0x30, 0x36, 0x32, 0x1E, 0x00
    DB  0x00, 0x00, 0x00, 0x00, 0x79, 0x6D, 0x79, 0x6D
    DB  0x78, 0x00, 0x72, 0xDB, 0xDB, 0xFA, 0xDA, 0x00

    ; #20
    DB  0x00, 0x00, 0x00, 0x00, 0xB7, 0xB6, 0xB7, 0xB6
    DB  0xE6, 0x00, 0x2F, 0x6C, 0xEF, 0xAC, 0x2F, 0x00
    DB  0x00, 0x00, 0x00, 0x00, 0x98, 0xD8, 0x98, 0xD8
    DB  0xD8, 0x00, 0x9E, 0x30, 0x3E, 0x06, 0xBC, 0x00


    ; #24
    DB  $64,$2A,$2E,$AA,$4A,$00,$00,$00
    DB  $00,$00,$00,$00,$00,$00,$00,$00
    DB  $E4,$AA,$EE,$8A,$8A,$00,$00,$00
    DB  $00,$00,$00,$00,$00,$00,$00,$00

    ; #28
    DB  $CE,$A8,$AE,$A8,$AE,$00,$00,$00
    DB  $00,$00,$00,$00,$00,$00,$00,$00
    DB  $EE,$88,$4E,$28,$EE,$00,$00,$00
    DB  $00,$00,$00,$00,$00,$00,$00,$00

    ; #32
    DB  $1D,$11,$1D,$11,$1D,$00,$00,$00
    DB  $00,$00,$00,$00,$00,$00,$00,$00
    DB  $8D,$51,$5D,$55,$4D,$00,$00,$00
    DB  $00,$00,$00,$00,$00,$00,$00,$00

    ; #36
    DB  $17,$14,$12,$11,$D7,$00,$00,$00
    DB  $00,$00,$00,$00,$00,$00,$00,$00
    DB  $50,$50,$70,$50,$50,$00,$00,$00
    DB  $00,$00,$00,$00,$00,$00,$00,$00

    ; #40
    DB  $1D,$11,$09,$05,$1D,$00,$00,$00
    DB  $00,$00,$00,$00,$00,$00,$00,$00
    DB  $DD,$55,$DD,$15,$15,$00,$00,$00
    DB  $00,$00,$00,$00,$00,$00,$00,$00

    ; #48
    DB  $97,$54,$52,$51,$57,$00,$00,$00
    DB  $00,$00,$00,$00,$00,$00,$00,$00
    DB  $50,$50,$70,$50,$50,$00,$00,$00
    DB  $00,$00,$00,$00,$00,$00,$00,$00

LOGO_ATTR_DATA:
    DB   32,  96,   0,   8
    DB   32, 128,   4,   8
    DB   64,  96,   8,   8
    DB   64, 128,  12,   8
    DB   90,  96,  16,  15
    DB   90, 128,  20,  15

LOGO_SE:
    DB  200
    DW  LOGO_SE_TRK1
    DW  LOGO_SE_TRK2
    DW  LOGO_SE_TRK3
LOGO_SE_TRK1:
    DB  217, %10, 214, 51, 4, 210, 51, 4, 214, 55, 4, 210, 55, 4, 214, 56
    DB  4, 210, 56, 4, 214, 58, 4, 210, 58, 4, 200, 0, 8, 214, 58, 8
    DB  60, 40, 213, 60, 4, 212, 60, 4, 200, 0, 32
    DB  255
LOGO_SE_TRK2:
    DB  217, %10, 214, 46, 4, 210, 46, 4, 214, 51, 4, 210, 51, 4, 214, 53
    DB  4, 210, 53, 4, 214, 55, 4, 210, 55, 4, 200, 0, 8, 214, 55, 8
    DB  56, 40, 213, 56, 4, 212, 56, 4, 200, 0, 32
    DB  255
LOGO_SE_TRK3:
    DB  218, 1, 217, %10, 214, 51, 4, 210, 51, 4, 214, 55, 4, 210, 55, 4, 214, 56
    DB  4, 210, 56, 4, 214, 58, 4, 210, 58, 4, 200, 0, 8, 214, 58, 8
    DB  60, 40, 213, 60, 4, 212, 60, 4, 200, 0, 32
    DB  255


SECTION data_user

LANGUAGE_ATTR_DATA:
    DB  160,  96,  24,  15
    DB  160, 128,  28,  15
