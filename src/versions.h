#ifndef FLASH_VERSIONS_H
#define FLASH_VERSIONS_H

#include <stdint.h>

struct version_number {
    uint8_t bootMajorVersion;
    uint8_t bootMinorVersion;
    uint8_t bootRevisionVersion;
    uint24_t bootBuildVersion;
};

struct version {
    struct version_number number;
    void *nop_location;
};

#define NUM_VERSIONS 3

extern const struct version versions[NUM_VERSIONS];

#endif //FLASH_VERSIONS_H
