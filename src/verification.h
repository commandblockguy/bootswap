#ifndef BOOTSWAP_VERIFICATION_H
#define BOOTSWAP_VERIFICATION_H

#include <stddef.h>
#include <stdbool.h>

uint24_t crc24(const uint8_t *buf, size_t len);

bool verify_interrupt_handlers(void);
bool verify_boot_calls(void);
bool verify_pre_m_version(void);

#endif //BOOTSWAP_VERIFICATION_H
