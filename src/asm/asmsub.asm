; license:MIT License
; copyright-holders:aburi6800 (Hitoshi Iwai)

; BIOS
WRTVRM              equ 0x004d
GTTRIG              equ 0x00d8
CHSNS               equ 0x009c
CHGET               equ 0x009f
KILBUF              equ 0x0156

; SUB-ROM BIOS(MSX2 leter)
SETPLT	            equ 0x014d
EXTROM              equ 0x015f

; work area
JIFFY               equ 0xfc9e

; VRAM address
VRAM_PTN_NAME_TBL1  equ 0x1800
VRAM_PTN_NAME_TBL3  equ 0x1A00


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
PUBLIC _put_message_direct
PUBLIC _clear_message
PUBLIC _getkeycode
PUBLIC _keywait
PUBLIC _buffer_check
PUBLIC _switch_bank
PUBLIC _check_region
PUBLIC _set_palette
PUBLIC _REGION


SECTION code_user


; ============================================================
; void put_message(void *dist_addr, void *value_addr) __naked;
;
; enter : uint8_t/c     Write start position (x)
;         uint8_t/b     Write start position (y)
;         uint16_t/de   first address of message address data(Japanese, English)
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
    ; get message data address
    push    de
    pop     hl

    ; check region
    ld      a, (REGION)
    or      a
    jr      z, put_message_L0

    inc     hl
    inc     hl

put_message_L0:
    ld      e, (hl)
    inc     hl
    ld      d, (hl)

put_message_L1:
    ; calcurate vram address
    ld      h, 0
    ld      l, b
    add     hl, hl  ; hl = hl * 2
    add     hl, hl  ; hl = hl * 4
    add     hl, hl  ; hl = hl * 8
    add     hl, hl  ; hl = hl * 16
    add     hl, hl  ; hl = hl * 32

    ld      b, 0
    add     hl, bc
    ld      bc, VRAM_PTN_NAME_TBL1
    add     hl, bc

put_message_L2:
    ; de = message data address
    ; hl = write vram address
    ld      a, (de)
    inc     de

    or      a
    jr      z, put_message_exit     ; 0x00 = end of data

    cp      0x01
    jr      z, play_music           ; 0x01 = effects(music)

    cp      0x02
    jr      z, play_music_nowait    ; 0x02 = effects(music_nowait)

    cp      0x03
    jp      z, play_se              ; 0x03 = effects(se)

    cp      0x0a
    jr      z, put_message_L3       ; 0x0a = line feed

    cp      0x0b
    jr      z, put_message_L5       ; 0x0b = wait & clear message

    cp      0x0c
    jr      z, put_message_L6       ; 0x0c = clear message

    cp      0xde
    jr      z, put_message_L4       ; 0xde = dakuten

    cp      0xdf
    jr      z, put_message_L4       ; 0xdf = handakuten

    call    put_message_L7
    jr      put_message_L2

put_message_L3:
    ; line feed
    ld      bc, 32
    adc     hl, bc

    ld      a, (REGION)
    or      a
    jr      nz, put_message_L31

    adc     hl, bc                  ; Two line feeds for Japanese

put_message_L31:
    ; set the first column of the next line
    ld      a, 0b11100000
    and     l
    ld      l, a

    ; bottom line check
    ld      a, h
    cp      0x1a
    jr      nz, put_message_L2

    ld      a, l
    cp      0xe0
    jr      nz, put_message_L2

    ; print wait message and clear message
    jr      put_message_L5

put_message_L4:
    ; put dakuten / handakuten (japanese)
    ld      bc, 32 + 1
    sbc     hl, bc
    call    put_message_L7

    add     hl, bc
    dec     hl
    jr      put_message_L2

put_message_L5:
    ; wait message
    call    keywait

put_message_L6:
    ; clear message
    call    clear_message

    ld      h, 0x1a                 ; reset write vram address   
    ld      l, 0x20

    jr      put_message_L2

put_message_L7:
    ; put character
    call    WRTVRM
    inc     hl

put_message_exit:
    ret


; ============================================================
; void put_message_direct(void *dist_addr, void *value_addr) __naked;
;
; enter : uint8_t/c     Write start position (x)
;         uint8_t/b     Write start position (y)
;         uint16_t/de   first address of message data
; exit  : void
; ============================================================
_put_message_direct:
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

put_message_direct:
    jp      put_message_L1


; ============================================================
; effects sub (music)
; ============================================================
play_music:
    push    hl
    call    music_select
    push    de
    call    SOUNDDRV_STOP
    call    SOUNDDRV_BGMPLAY

play_music_L1:
    ld      a, (SOUNDDRV_STATE)
    or      a
    jr      nz, play_music_L1

play_music_exit:
    pop     de
    pop     hl
    jp      put_message_L2


; ============================================================
; effects sub (music_nowait)
; ============================================================
play_music_nowait:
    push    hl
    call    music_select
    push    de

    call    SOUNDDRV_BGMPLAY

play_music_nowait_exit:
    pop     de
    pop     hl
    jp      put_message_L2


; ============================================================
; effects sub (se)
; ============================================================
play_se:
    push    hl
    call    music_select
    push    de
    call    SOUNDDRV_SFXPLAY

play_se_exit:
    pop     de
    pop     hl
    jp      put_message_L2

; ============================================================
; music select
; ============================================================
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
    push    bc
    push    de
    push    hl

    ld      hl, VRAM_PTN_NAME_TBL3
    ld      a, 0x20
    ld      b, 8                    ; 8 lines

clear_message_L1:
    push    bc
    ld      b, 32                   ; 32 columns

clear_message_L2:
    call    WRTVRM
    inc     hl
    djnz    clear_message_L2

    pop     bc
    djnz    clear_message_L1

    ld      a, $0a
    call    timewait

clear_message_exit:
    pop     hl
    pop     de
    pop     bc
    ret


; ============================================================
; uint8_t getkeycode() __naked;
;
; enter : void
; exit  : uint8_t       key code
; ============================================================
_getkeycode:
getkeycode:
    call    KILBUF
    call    CHGET
    ld      h, a    ; return parameter
    ld      l, 0

_getkeycode_exit:
    ret


; ============================================================
; void keywait() __naked;
;
; enter : void
; exit  : void
;
; note  : The screen is not erased; clear_message should be executed.
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
    call    put_message_direct

    call    buffer_check

keywait_L1:
    ld      a, 0                    ; 0 = keyboard
    call    GTTRIG
    or      a
    jp      z, keywait_L1

keywait_exit:
    pop     hl
    pop     de
    pop     bc
    ret


; ============================================================
; timewait
;
; enter : uint8_t/a     Write time(1/60 second increments)
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
    call    CHSNS
    ret     z
    call    CHGET
    jr      _buffer_check


; ============================================================
; void switch_bank(uint8_t bank_no) __naked;
;
; enter : uint8_t/a     switch bank no
; exit  : void
; ============================================================
_switch_bank:
    di

    ld      hl, 2
    add     hl, sp
    ld      a, (hl)     ; get arg value

switch_bank:
    ld      hl, 0x7000  ; ASCII16 Mapper control port(0x8000～0x8fff)
    ld      (hl), a

    ei
    ret


; ============================================================
; void check_region() __naked;
;
; enter : void
; exit  : void
; ============================================================
_check_region:
check_region:
    xor     a
    ld      h, 0x00
    ld      l, 0x2b
    or      (hl)
    ld      h, 0x00
    ld      l, 0x2c
    or      (hl)

    or      a
    jr      z, check_region_L1
    ld      a, 0x01

check_region_L1:
    ld      (REGION), a

check_region_exit:
    ret


; ============================================================
; void set_palette() __naked;
;
; enter : void
; exit  : void
; ============================================================
_set_palette:
set_palette:
    ; check MSX version
	ld		a, (0x002d)
	or		a
	ret		z

	ld		ix, SETPLT  ; Set BIOS entry address
	ld 		hl, _PALLETE_DATA
	ld 		d, 0		; pallet no

set_palette_1:
	ld		a, (hl)
	inc		hl
	ld		e, (hl)
	inc		hl
	call	EXTROM

	inc		d
	ld		a, 16
	sub		d
	jr		nz, set_palette_1

set_palette_exit:
    ret


SECTION rodata_user

_WAAT_MESSAGE_TEXT:
    db  0x5b, 0x50, 0x55, 0x53, 0x48, 0x20, 0x53, 0x50, 0x41, 0x43, 0x45, 0x5d, 0x00

_PALLETE_DATA:
	; +0 : RED(4bit) + blue(4bit)
	; +1 : 0b0000 + green(4bit)
	db  0x00, 0x00	; 0
	db  0x00, 0x00	; 1
	db  0x22, 0x05	; 2
	db  0x33, 0x06	; 3
	db  0x26, 0x02	; 4
	db  0x47, 0x03	; 5
	db  0x52, 0x03	; 6
	db  0x37, 0x06	; 7
	db  0x62, 0x03	; 8
	db  0x73, 0x04	; 9
	db  0x63, 0x05	; 10
	db  0x64, 0x06	; 11
	db  0x22, 0x04	; 12
	db  0x55, 0x03	; 13
	db  0x66, 0x06	; 14
	db  0x77, 0x07	; 15


SECTION bss_user
_REGION:
REGION:
    db  00                  ; 00=Japanese, 01=English
