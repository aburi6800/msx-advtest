; license:MIT License
; copyright-holders:aburi6800 (Hitoshi Iwai)

SECTION code_user

PUBLIC _switch_bank


; ============================================================
; void switch_bank(void *bank_no) __naked;
;
; enter : uint8_t   switch bank no
; exit  : void
; ============================================================

_switch_bank:
    ld      hl, 2
    add     hl, sp
    ld      a, (hl)     ; get arg value

    ld      hl, 0x7000  ; ASCII16 マッパーの制御ポート(0x8000～0x8fff)
    ld      (hl), a
    ret
