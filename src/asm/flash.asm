public _flash_unlock
public _flash_lock
public _get_flash_lock_status
public _flash_sequence
public _write_bytes
public write_byte
public _erase_sector
public _reset_all_ipbs
public _set_boot_ipbs


_flash_unlock:
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

_flash_lock:
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

_get_flash_lock_status:
    ld  bc,$28
    jp  read_port

_flash_sequence:
    pop de
    pop hl
    push    hl
    push    de
    jp  (hl)

_write_bytes:
    ld  iy,0
    add iy,sp
    ld  de,(iy+3)
    ld  hl,(iy+6)
    ld  bc,(iy+9)
write_bytes:
    call    write_byte
    inc hl
    inc de
    dec bc
    push	hl
    sbc	hl,hl
    adc	hl,bc
    pop	hl
    jr  nz,write_bytes
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

_erase_sector:
    pop de
    pop hl
    push    hl
    push    de
; this bit was taken from the bootcode
; maybe actually copy it from there to avoid copyright issues?
    ld a,$AA
    ld ($000AAA),a
    ld a,$55
    ld ($000555),a
    ld a,$80
    ld ($000AAA),a
    ld a,$AA
    ld ($000AAA),a
    ld a,$55
    ld ($000555),a
    ld a,$30
    ld (hl),a
.read_1:
    ld a,(hl)
    and a,$08
    jr nz,.read_1
    ld a,(hl)
    and a,$40
    ld b,a
.read_2:
    ld a,(hl)
    ld c,a
    and a,$40
    cp a,b
    ret z
    ld a,c
    and a,$40
    ld b,a
    ld a,c
    and a,$20
    jr z,.read_2
    ld a,(hl)
    and a,$40
    ld b,a
    ld a,(hl)
    and a,$40
    cp a,b
    ret z
    ld a,$F0
    ld (hl),a
    or a,a
    ret

_reset_all_ipbs:
    call    enter_ipb_program
    ld  a,$80
    ld  ($00),a
    ld  a,$30
    ld  ($00),a

    ld  hl,24000*71/33
.loop:
    dec hl
    add	hl,de
    or	a,a
    sbc	hl,de
    jr  nz,.loop
    jp  exit_ipb_program

_set_boot_ipbs:
    ld  b,9
    ld  hl,0
    ld  de,8192
.set_ipb:
    call    enter_ipb_program
    ld  a,$A0
    ld  ($000),a
    ld  a,$00
    ld  (hl),a
    add hl,de
    call    wait_500_us
    call    exit_ipb_program
    djnz    .set_ipb
    ret

wait_500_us:
    push    hl
    ld  hl,24000/33
.loop:
    dec hl
    add	hl,de
    or	a,a
    sbc	hl,de
    jr  nz,.loop
    pop hl
    ret

enter_ipb_program:
    ld  a,$AA
    ld  ($AAA),a
    ld  a,$55
    ld  ($555),a
    ld  a,$C0
    ld  ($AAA),a
    ret

exit_ipb_program:
    ld  a,$90
    ld  ($00),a
    xor a,a
    ld  ($00),a
    ret

extern read_port
extern write_port