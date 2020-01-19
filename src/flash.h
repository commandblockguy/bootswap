#ifndef FLASH_FLASH_H
#define FLASH_FLASH_H

#include <stdbool.h>

bool write_sector(void *sector, const void *data);

void unlock_bootcode(void);
void lock_bootcode(void);

#endif //FLASH_FLASH_H
