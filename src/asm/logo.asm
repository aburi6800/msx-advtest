; license:MIT License
; copyright-holders:aburi6800 (Hitoshi Iwai)

PUBLIC _boot_logo

WRVDP   EQU     0x0007
WRTVRM  EQU     0x004d
FILVRM  EQU     0x0056
LDIRVM  EQU     0x005c
CHGCLR  EQU     0x0062
CLRSPR  EQU     0x0069
INIT32  EQU     0x006f
SETGRP  EQU     0x007e
SNSMAT  EQU     0x0141

FORCLR  EQU     0xf3e9
BAKCLR  EQU     0xf3ea
BDRCLR  EQU     0xf3eb
JIFFY   EQU     0xfc9e


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


    ; clear pattern name table
    ld      a, 0xfe
    ld      bc, 0x0100
    ld      hl, 0x1800
    call    FILVRM
    ld      bc, 0x0100
    ld      hl, 0x1900
    call    FILVRM
    ld      bc, 0x0100
    ld      hl, 0x1A00
    call    FILVRM

    ; clear color table
    ld      a, 0x00
    ld      bc, 0x0800
    ld      hl, 0x2000
    call    FILVRM
    ld      bc, 0x0800
    ld      hl, 0x2800
    call    FILVRM
    ld      bc, 0x0800
    ld      hl, 0x3000
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
    ld      hl, LOGO_PATTERN        ; source address(ram)
    ld      de, 0x3800              ; dist address(vram)
    ld      bc, 192                 ; data length
    call    LDIRVM

    ; set sprite attribute tablle
    ld      hl, 0x1b00              ; sprite attribute table address
    ld      de, LOGO_DATA           ; data address

    ld      b, 24
BOOT_LOGO_1:
    ld      a, (de)                 ; get logo data
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
    ld      a, (_REGION)
    cp      1
    jr      z, BOOT_LOGO_4          ; skip when english

    ld      a, 7
    call    SNSMAT
    cp      0b10111111              ; SELECT key
    jr      nz, BOOT_LOGO_4

    ; change english
    ld      a, 1
    ld      (_REGION), a            

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

BOOT_LOGO_4:
    ld      a, (JIFFY)              ; jiffy load
    cp      0xf0                    ; 4 seconds
    jr      nz, BOOT_LOGO_3

    call    CLRSPR

    ret


SECTION rodata_user
LOGO_PATTERN:
    DB  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    DB  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01
    DB  0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x01, 0x01
    DB  0x01, 0x13, 0x1B, 0x3F, 0x7F, 0xFF, 0xFF, 0xFD

    DB  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80
    DB  0x80, 0xC0, 0xC2, 0xE2, 0xE7, 0xEF, 0xFF, 0xFF
    DB  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    DB  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0

    DB  0x03, 0x03, 0x03, 0x07, 0x07, 0x06, 0x06, 0x06
    DB  0x06, 0x06, 0x06, 0x06, 0x03, 0x01, 0x00, 0x00
    DB  0xFD, 0xFC, 0xFE, 0x80, 0x00, 0x30, 0x30, 0xFC
    DB  0xFC, 0x30, 0x30, 0x03, 0x07, 0xFF, 0xFF, 0x00

    DB  0xFF, 0xFF, 0x7F, 0x20, 0x00, 0x06, 0x06, 0x19
    DB  0x19, 0x06, 0x06, 0xE0, 0xF0, 0xFF, 0xFF, 0x00
    DB  0xC0, 0xE0, 0xE0, 0xF0, 0x70, 0x30, 0x30, 0xB0
    DB  0xB0, 0x30, 0x30, 0x30, 0x60, 0xC0, 0x80, 0x00

    DB  0x00, 0x00, 0x00, 0x00, 0x0E, 0x1B, 0x1B, 0x1F
    DB  0x1B, 0x00, 0x1E, 0x30, 0x36, 0x32, 0x1E, 0x00
    DB  0x00, 0x00, 0x00, 0x00, 0x79, 0x6D, 0x79, 0x6D
    DB  0x78, 0x00, 0x72, 0xDB, 0xDB, 0xFA, 0xDA, 0x00

    DB  0x00, 0x00, 0x00, 0x00, 0xB7, 0xB6, 0xB7, 0xB6
    DB  0xE6, 0x00, 0x2F, 0x6C, 0xEF, 0xAC, 0x2F, 0x00
    DB  0x00, 0x00, 0x00, 0x00, 0x98, 0xD8, 0x98, 0xD8
    DB  0xD8, 0x00, 0x9E, 0x30, 0x3E, 0x06, 0xBC, 0x00

LOGO_DATA:
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
