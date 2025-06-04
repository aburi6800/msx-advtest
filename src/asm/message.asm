; license:MIT License
; copyright-holders:aburi6800 (Hitoshi Iwai)

#define MSX_WRTVRM          0x004d
#define BEEP                0x00c0
#define VRAM_PTN_NAME_TBL1  0x1800


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


SECTION code_user

PUBLIC _put_message
PUBLIC _clear_message


; ============================================================
; void put_message(void *dist_addr, void *value_addr) __naked;
;
; enter : uint8_t   Write start position (x)
;         uint8_t   Write start position (y)
;         uint16_t  first address of display string data
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

_put_message_L1:
    ld      a, (de)
    inc     de

    or      a
    jr      z, _put_message_exit    ; 0x00 = end of data

    cp      0x01
    jr      z, play_music           ; 0x01 = effects(music)

    cp      0x02
    jr      z, play_se              ; 0x02 = effects(se)

    cp      0x0a
    jr      z, _put_message_L2      ; 0x0a = line feed

    cp      0xde
    jr      z, _put_message_L3      ; 0xde = dakuten

    cp      0xdf
    jr      z, _put_message_L3      ; 0xdf = handakuten

    call    _put_message_L4
    jr      _put_message_L1

_put_message_L2:
    ld      bc, 32
    adc     hl, bc
    adc     hl, bc

    ld      a, 0b11100000
    and     l
    ld      l, a

    jr      _put_message_L1

_put_message_L3:
    ld      bc, 32 + 1
    sbc     hl, bc

    call    _put_message_L4

    add     hl, bc
    dec     hl
    jr      _put_message_L1

_put_message_L4:
    call    MSX_WRTVRM
    inc     hl

_put_message_exit:
    ret


; ============================================================
; effects sub (music)
; ============================================================
play_music:
    push    hl
    call    music_select
    call    SOUNDDRV_BGMPLAY

play_music_1:
    ld      a, (SOUNDDRV_STATE)
    or      a
    jr      nz, play_music_1

play_music_exit:
    pop     hl
    jr      _put_message_L1


; ============================================================
; effects sub (se)
; ============================================================
play_se:
    push    hl
    call    music_select
    call    SOUNDDRV_SFXPLAY

play_se_exit:
    pop     hl
    jr      _put_message_L1


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
    ld      hl, VRAM_PTN_NAME_TBL1 + (16 * 32)
    ld      a, 0x20
    ld      b, 8    ; 8 lines
_clear_message_L1:
    push    bc
    ld      b, 32   ; 32 columns
_clear_message_L2:
    call    MSX_WRTVRM
    inc     hl
    djnz    _clear_message_L2

    pop     bc
    djnz    _clear_message_L1
_clear_message_exit:
    ret
