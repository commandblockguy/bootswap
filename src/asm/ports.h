#ifndef FLASH_PORTS_H
#define FLASH_PORTS_H

void set_priv(void);
void reset_priv(void);

uint8_t priv_upper(void);

void priv_copy(void *dst, void *src, size_t size);

#endif //FLASH_PORTS_H
