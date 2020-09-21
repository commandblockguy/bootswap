public read_port
public write_port
public _set_priv
public _reset_priv
public _priv_upper

; this is hella inefficient, but who actually cares?
read_port:
	ld	a,mb
	push	af
	ld	a,.z80 shr 16
	ld	mb,a
	jp.sis	.z80 and $FFFF
assume adl = 0
.z80:
	in	e,(bc)
	jp.lil	.continue
assume adl = 1
.continue:
	pop	af
	ld	mb,a
	ld	a,e
	ret

write_port:
	ld	e,a
	ld	a,mb
	push	af
	ld	a,.z80 shr 16
	ld	mb,a
	jp.sis	.z80 and $FFFF
assume adl = 0
.z80:
	out	(bc),e
	jp.lil	.continue
assume adl = 1
.continue:
	pop	af
	ld	mb,a
	ret

_set_priv:
	ld	bc,$1d
	call	read_port
	ld	(priv_bkp),a
	ld	a,$FF
	call	write_port
	ld	bc,$1e
	call	read_port
	ld	(priv_bkp+1),a
	ld	a,$FF
	call	write_port
	ld	bc,$1f
	call	read_port
	ld	(priv_bkp+2),a
	ld	a,$FF
	jp	write_port

_reset_priv:
	ld	bc,$1d
	ld	a,(priv_bkp)
	call	write_port
	ld	bc,$1e
	ld	a,(priv_bkp+1)
	call	write_port
	ld	bc,$1f
	ld	a,(priv_bkp+2)
	jp	write_port

_priv_upper:
	ld	bc,$1d
	call	read_port
	ret

priv_bkp:
rl	1