#ifndef BOOTSWAP_PATCH_H
#define BOOTSWAP_PATCH_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

bool patch(void *location, const void *replacement, const void *old, size_t size);

#endif //BOOTSWAP_PATCH_H
