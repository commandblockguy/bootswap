    public _asmprgm
    public _seq
    public _mod_loc

heapBot			:= 0D1887Ch
scrapMem        := 0D02AD7h
_CheckHardware  := 015F3h
ramCodeTop      := 0D19C7Ch

_asmprgm:
    ld  bc,$1d
    call    read_port
    ld  (priv_bkp),a
    ld  a,$FF
    call    write_port
    ld  bc,$1e
    call    read_port
    ld  (priv_bkp+1),a
    ld  a,$FF
    call    write_port
    ld  bc,$1f
    call    read_port
    ld  (priv_bkp+2),a
    ld  a,$FF
    call    write_port

    call    flash_unlock

    ld  hl,(_seq)
    call    jp_hl

; enter ipb program mode
    ld  a,$AA
    ld  ($AAA),a
    ld  a,$55
    ld  ($555),a
    ld  a,$C0
    ld  ($AAA),a

; reset all ipbs
    ld  a,$80
    ld  ($00),a
    ld  a,$30
    ld  ($00),a

; exit ipb mode
    ld  a,$90
    ld  ($00),a
    ld  a,$00
    ld  ($00),a

; write bytes
    ld  de,(_mod_loc)
    ld  hl,overwrite
    ld  b,overwrite_size
    call    write_bytes

; write our actual byte
    ld  de,$08D6FC
    ld  hl,overwrite ; for a 0
    call    write_byte

; enter ipb program mode
    ld  a,$AA
    ld  ($AAA),a
    ld  a,$55
    ld  ($555),a
    ld  a,$C0
    ld  ($AAA),a

; reactivate ipbs
    ld  b,8
    ld  hl,0
set_ipb:
    ld  a,$A0
    ld  ($000),a
    ld  a,$00
    ld  (hl),a
    inc hl
    djnz    set_ipb

    call    flash_lock

; reset priveledged code
    ld  bc,$1d
    ld  a,(priv_bkp)
    call    write_port
    ld  bc,$1e
    ld  a,(priv_bkp+1)
    call    write_port
    ld  bc,$1f
    ld  a,(priv_bkp+2)
    call    write_port
    
    jr  $

read_port:
	ld	de,$C978ED
	ld	hl,heapBot - 3
	ld	(hl),de
jp_hl:
	jp	(hl)

write_port:
	ld	de,$C979ED
	ld	hl,heapBot - 3
	ld	(hl),de
	jp	(hl)

add_ldir:
	ld	de,$C9B0ED
	ld	hl,heapBot - 3
	ld	(hl),de
    ret

flash_unlock:
	ld	bc,$24
	ld	a,$8c
	call	write_port
	ld	bc,$06
	call	read_port
	or	a,4
	call	write_port
	ld	bc,$28
	ld	a,$4
	jp	write_port

flash_lock:
	ld	bc,$28
	xor	a,a
	call	write_port
	ld	bc,$06
	call	read_port
	res	2,a
	call	write_port
	ld	bc,$24
	ld	a,$88
	jp	write_port

write_bytes:
    call    write_byte
    inc hl
    inc de
    djnz    write_bytes
    ret

write_byte:
    ld  a,$AA
    ld  ($AAA),a
    ld  a,$55
    ld  ($555),a
    ld  a,$A0
    ld  ($AAA),a
    ld  a,(hl)
    ld  (de),a
    ret

; actual data there is DDE5DD21000000DD...
;                      00210100000000C9
; ld hl is 0x21, ret is 0xC9
overwrite:
    nop
    ld  hl,1
    nop
    nop
    ret
overwrite_size := $-overwrite

_seq:
rl  1

_mod_loc:
rl  1

priv_bkp:
rl 1
