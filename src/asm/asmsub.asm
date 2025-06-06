; license:MIT License
; copyright-holders:aburi6800 (Hitoshi Iwai)

#define MSX_WRTVRM              0x004d
#define MSX_GTTRIG              0x00d8
#define MSX_CHSNS               0x009c
#define MSX_CHGET               0x009f
#define MSX_KILBUF              0x0156

#define VRAM_PTN_NAME_TBL1      0x1800
#define JIFFY                   0xfc9e

EXTERN SOUNDDRV_INIT
EXTERN SOUNDDRV_EXEC
EXTERN SOUNDDRV_BGMPLAY
EXTERN SOUNDDRV_SFXPLAY
EXTERN SOUNDDRV_STOP
EXTERN SOUNDDRV_PAUSE
EXTERN SOUNDDRV_RESUME
EXTERN SOUNDDRV_STATE

EXTERN _MUSIC_TITLE
EXTERN _MUSIC_OVER
EXTERN _MUSIC_ENDING
EXTERN _SE_01



PUBLIC _put_message
PUBLIC _clear_message
PUBLIC _getkeycode
PUBLIC _keywait
PUBLIC _buffer_check
PUBLIC _switch_bank


SECTION code_user


; ============================================================
; void put_message(void *dist_addr, void *value_addr) __naked;
;
; enter : uint8_t/c     Write start position (x)
;         uint8_t/b     Write start position (y)
;         uint16_t/de   first address of display string data
; exit  : void
; ============================================================
_put_message:

    ld      hl, 2
    add     hl, sp
    ld      e, (hl) ; get arg3 value
    inc     hl
    ld      d, (hl)
    inc     hl
    ld      b, (hl) ; get arg2 value
    inc     hl      
    inc     hl      
    ld      c, (hl) ; get arg1 value

put_message:
    ld      h, 0
    ld      l, b
    add     hl, hl  ; hl = hl * 2
    add     hl, hl  ; hl = hl * 4
    add     hl, hl  ; hl = hl * 8
    add     hl, hl  ; hl = hl * 16
    add     hl, hl  ; hl = hl * 32

    ld      b, 0
    add     hl, bc
    add     hl, VRAM_PTN_NAME_TBL1

put_message_L1:
    ld      a, (de)
    inc     de

    or      a
    jr      z, put_message_exit     ; 0x00 = end of data

    cp      0x01
    jr      z, play_music           ; 0x01 = effects(music)

    cp      0x02
    jr      z, play_se              ; 0x02 = effects(se)

    cp      0x0a
    jr      z, put_message_L2       ; 0x0a = line feed

    cp      0xde
    jr      z, put_message_L3       ; 0xde = dakuten

    cp      0xdf
    jr      z, put_message_L3       ; 0xdf = handakuten

    call    put_message_L4
    jr      put_message_L1

put_message_L2:
    ld      bc, 32
    adc     hl, bc
    adc     hl, bc

    ld      a, 0b11100000
    and     l
    ld      l, a

    ld      a, h
    cp      0x1a
    jr      nz, put_message_L1

    ld      a, l
    cp      0xe0
    jr      nz, put_message_L1

    call    keywait

    ld      h, 0x1a
    ld      l, 0x20

    jr      put_message_L1

put_message_L3:
    ld      bc, 32 + 1
    sbc     hl, bc

    call    put_message_L4

    add     hl, bc
    dec     hl
    jr      put_message_L1

put_message_L4:
    call    MSX_WRTVRM
    inc     hl

put_message_exit:
    ret


; ============================================================
; effects sub (music)
; ============================================================
play_music:
    push    hl
    call    music_select
    call    SOUNDDRV_BGMPLAY

play_music_L1:
    ld      a, (SOUNDDRV_STATE)
    or      a
    jr      nz, play_music_L1

play_music_exit:
    pop     hl
    jr      put_message_L1


; ============================================================
; effects sub (se)
; ============================================================
play_se:
    push    hl
    call    music_select
    call    SOUNDDRV_SFXPLAY

play_se_exit:
    pop     hl
    jp      put_message_L1


music_select:
    ld      a, (de)
    inc     de

    dec     a
    jr      z, music_select_1

    dec     a
    jr      z, music_select_2

    dec     a
    jr      z, music_select_3

    dec     a
    jr      z, music_select_4

    ret

music_select_1:
    ld      hl, _MUSIC_TITLE
    ret

music_select_2:
    ld      hl, _MUSIC_OVER
    ret

music_select_3:
    ld      hl, _MUSIC_ENDING
    ret

music_select_4:
    ld      hl, _SE_01
    ret


; ============================================================
; void clear_messager() __naked;
;
; enter : void
; exit  : void
; ============================================================
_clear_message:
clear_message:
    ld      hl, VRAM_PTN_NAME_TBL1 + (16 * 32)
    ld      a, 0x20
    ld      b, 8    ; 8 lines

clear_message_L1:
    push    bc
    ld      b, 32   ; 32 columns

clear_message_L2:
    call    MSX_WRTVRM
    inc     hl
    djnz    clear_message_L2

    pop     bc
    djnz    clear_message_L1

clear_message_exit:
    ret


; ============================================================
; uint8_t getkeycode() __naked;
;
; enter : void
; exit  : uint8_t       key code
; ============================================================
_getkeycode:
getkeycode:
    call    MSX_KILBUF
    call    MSX_CHGET
    ld      h, a    ; return parameter
    ld      l, 0

_getkeycode_exit:
    ret


; ============================================================
; void keywait() __naked;
;
; enter : void
; exit  : void
; ============================================================
_keywait:
keywait:
    push    bc
    push    de
    push    hl

    ld      a, 0x20
    call    timewait

    ld      c, 20                   ; write start position (x)
    ld      b, 23                   ; write start position (y)
    ld      de, _WAAT_MESSAGE_TEXT  ; first address of display string data
    call    put_message

    call    buffer_check

keywait_L1:
    ld      a, 0                    ; 0 = keyboard
    call    MSX_GTTRIG
    or      a
    jp      z, keywait_L1

    call    clear_message
    ld      a, $0a
    call    timewait

keywait_exit:
    pop     hl
    pop     de
    pop     bc
    ret


; ============================================================
; timewait
;
; enter : a             Write time(1/60 second increments)
; exit  : void
; ============================================================
_timewait:
    ld      hl, 2
    add     hl, sp
    ld      a, (hl)     ; get arg value

timewait:
    ld      b, a

    xor     a
    ld      (JIFFY), a

timewait_L1:
    ld      a, (JIFFY)
    cp      b
    jr      nz, timewait_L1

    ret


; ============================================================
; void buffer_check() __naked;
;
; enter : void
; exit  : void
; ============================================================
_buffer_check:
buffer_check:
    call    MSX_CHSNS
    ret     z
    call    MSX_CHGET
    jr      _buffer_check


; ============================================================
; void switch_bank(uint8_t bank_no) __naked;
;
; enter : uint8_t/a     switch bank no
; exit  : void
; ============================================================

_switch_bank:
    ld      hl, 2
    add     hl, sp
    ld      a, (hl)     ; get arg value

switch_bank:
    ld      hl, 0x7000  ; ASCII16 Mapper control port(0x8000～0x8fff)
    ld      (hl), a
    ret


SECTION rodata_user

_WAAT_MESSAGE_TEXT:
    db      0x5b, 0x50, 0x55, 0x53, 0x48, 0x20, 0x53, 0x50, 0x41, 0x43, 0x45, 0x5d, 0x00
