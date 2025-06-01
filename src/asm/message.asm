; license:MIT License
; copyright-holders:aburi6800 (Hitoshi Iwai)

#define MSX_WRTVRM          0x004d
#define BEEP                0x00c0
#define VRAM_PTN_NAME_TBL1  0x1800

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
