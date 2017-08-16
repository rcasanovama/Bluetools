#ifndef __BLUETOOTH_PBAP_H
#define __BLUETOOTH_PBAP_H

#if defined(__cplusplus)
extern "C"
{
#endif

#include <stdint.h>

#define __UUID_PBAP_bytes    \
{                            \
    0x79, 0x61, 0x35, 0xF0,  \
    0xF0, 0xC5, 0x11, 0xD8, 0x09, 0x66,   \
    0x08, 0x00, 0x20, 0x0C, 0x9A, 0x66    \
}

#define UUID_PBAP ((uint8_t []) __UUID_PBAP_bytes)

#if defined(__cplusplus)
}
#endif

#endif // __BLUETOOTH_PBAP_H
