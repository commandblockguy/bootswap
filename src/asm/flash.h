#ifndef FLASH_H
#define FLASH_H

void flash_unlock(void);
void flash_lock(void);
void flash_sequence(void *sequence);

void reset_all_ipbs(void);
void set_boot_ipbs(void);

void write_bytes(void *dest, const void *src, size_t size);

void erase_sector(void *sector);

bool uses_new_flash(void);

enum flash_lock_status {
    LOCKED = 0x00,
    PARTIALLY_UNLOCKED = 0x04,
    FULLY_UNLOCKED = 0x0C
};

uint8_t get_flash_lock_status(void);

#endif //FLASH_FLASH_H
