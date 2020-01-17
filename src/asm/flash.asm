public _flash_unlock
public _flash_lock
public _flash_sequence
public _write_bytes
public write_byte
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

_reset_all_ipbs:
    call    enter_ipb_program
    ld  a,$80
    ld  ($00),a
    ld  a,$30
    ld  ($00),a
    jp  exit_ipb_program

_set_boot_ipbs:
    call    enter_ipb_program
    ld  b,8
    ld  hl,0
set_ipb:
    ld  a,$A0
    ld  ($000),a
    ld  a,$00
    ld  (hl),a
    inc hl
    djnz    set_ipb
    jp  exit_ipb_program

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
    ld  a,$00
    ld  ($00),a
    ret

extern read_port
extern write_port