; license:MIT License
; copyright-holders:aburi6800 (Hitoshi Iwai)
;SECTION rodata_user
SECTION BANK_01

PUBLIC _FONT_PTN_TBL
PUBLIC _FONT_COL_TBL

_FONT_PTN_TBL:
INCBIN "../../resources/bank01/misaki.chr.plet5"

_FONT_COL_TBL:
INCBIN "../../resources/bank01/misaki.clr.plet5"
