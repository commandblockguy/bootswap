#ifndef FLASH_FLASH_H
#define FLASH_FLASH_H

#include <stdbool.h>

void write_sector(void *sector, const void *data);

void unlock_bootcode(void);
void lock_bootcode(void);

void boot_code_to_vram(void);
bool vram_to_boot_code(void);

#endif //FLASH_FLASH_H
