#ifndef FLASH_FLASH_H
#define FLASH_FLASH_H

void flash_unlock(void);
void flash_lock(void);
void flash_sequence(void *sequence);

void reset_all_ipbs(void);
void set_boot_ipbs(void);

void write_bytes(void *dest, const void *src, size_t size);

#endif //FLASH_FLASH_H
