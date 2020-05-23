#ifndef BOOTSWAP_PATCH_H
#define BOOTSWAP_PATCH_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define NUM_PATCHES 4

struct patch {
    size_t size;
    const uint8_t *patched_data;
    const uint8_t *unpatched_data;
};

extern const struct patch patches[NUM_PATCHES];

bool apply_patch(void *location, const void *replacement, const void *old, size_t size);

#endif //BOOTSWAP_PATCH_H
