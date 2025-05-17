; pletter v0.5c msx unpacker
; Original source : Copyright (c) 2002-2003 Team Bomba
; z88dk patch     : Copyright (C) 2025 aburi6800 (Hitoshi Iwai)

; call unpack with hl pointing to some pletter5 data, and de pointing to the destination.
; changes all registers

; define lengthindata when the original size is written in the pletter data

;  define LENGTHINDATA

;  module pletter

SECTION code_user
PUBLIC _unpack

;  macro GETBIT
MACRO GETBIT
  add a,a
  call z,getbit
;  endmacro
ENDM

;  macro GETBITEXX
MACRO GETBITEXX
  add a,a
  call z,getbitexx
;  endmacro
ENDM

_unpack:

  ; Get call parameters from z88dk C.
  ; Not necessary when calling from asm.
  pop bc
  pop de
  pop hl
  push bc


  ifdef LENGTHINDATA
  inc hl
  inc hl
  endif

  ld a,(hl)
  inc hl
  exx
  ld de,0
  add a,a
  inc a
  rl e
  add a,a
  rl e
  add a,a
  rl e
  rl e
  ld hl,modes
  add hl,de
  ld e,(hl)
  ld ixl,e
  inc hl
  ld e,(hl)
  ld ixh,e
  ld e,1
  exx
  ld iy,loop
literal:
  ldi
loop:
  GETBIT
  jr nc,literal
  exx
  ld h,d
  ld l,e
getlen:
  GETBITEXX
  jr nc,lenok
lus:
  GETBITEXX
  adc hl,hl
  ret c
  GETBITEXX
  jr nc,lenok
  GETBITEXX
  adc hl,hl
  ret c
  GETBITEXX
  jp c,lus
lenok:
  inc hl
  exx
  ld c,(hl)
  inc hl
  ld b,0
  bit 7,c
  jp z,offsok
  jp (ix)

mode6:
  GETBIT
  rl b
mode5:
  GETBIT
  rl b
mode4:
  GETBIT
  rl b
mode3:
  GETBIT
  rl b
mode2:
  GETBIT
  rl b
  GETBIT
  jr nc,offsok
  or a
  inc b
  res 7,c
offsok:
  inc bc
  push hl
  exx
  push hl
  exx
  ld l,e
  ld h,d
  sbc hl,bc
  pop bc
  ldir
  pop hl
  jp (iy)

getbit:
  ld a,(hl)
  inc hl
  rla
  ret

getbitexx:
  exx
  ld a,(hl)
  inc hl
  exx
  rla
  ret

modes:
  defw offsok
  defw mode2
  defw mode3
  defw mode4
  defw mode5
  defw mode6

;  endmodule

;eof