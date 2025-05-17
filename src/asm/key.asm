; license:MIT License
; copyright-holders:hitoshi Iwai

#define MSX_CHSNS           0x009c
#define MSX_CHGET           0x009f
#define MSX_GTTRIG          0x00d8
#define MSX_KILBUF          0x0156

SECTION code_user
PUBLIC _getkeycode
PUBLIC _keywait


; ============================================================
; uint8_t getkeycode() __naked;
;
; enter : void
; exit  : uint8_t   key code
; ============================================================
_getkeycode:
;    call    buffer_chk
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
    call    buffer_chk

_keywait_L1:
    ld      a, 0    ; 0 = keyboard
    call    MSX_GTTRIG
    or      a
    jp      z, _keywait_L1
_keywait_exit:
    ret


; ============================================================
; keyboard buffer check
; ============================================================
buffer_chk:
    call    MSX_CHSNS
    jp      nz, _getkeycode
    ret
