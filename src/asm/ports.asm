public read_port
public write_port
public priv_ldir
public _set_priv
public _reset_priv
public _priv_upper

heapBot			:= 0D1887Ch

read_port:
	ld	de,$C978ED
	ld	hl,heapBot - 3
	ld	(hl),de
	jp	(hl)

write_port:
	ld	de,$C979ED
	ld	hl,heapBot - 3
	ld	(hl),de
	jp	(hl)

priv_ldir:
	ld	de,$C9B0ED
	ld	hl,heapBot - 3
	ld	(hl),de
    ret

_set_priv:
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
    jp  write_port

_reset_priv:
    ld  bc,$1d
    ld  a,(priv_bkp)
    call    write_port
    ld  bc,$1e
    ld  a,(priv_bkp+1)
    call    write_port
    ld  bc,$1f
    ld  a,(priv_bkp+2)
    jp  write_port

_priv_upper:
    ld  bc,$1d
    call    read_port
    ret

priv_bkp:
rl  1