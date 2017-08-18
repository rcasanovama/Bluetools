#ifndef __BLUETOOTH_OBEX_UTILS_H
#define __BLUETOOTH_OBEX_UTILS_H

#if defined(__cplusplus)
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <endian.h>
#include <assert.h>

#include "obex-types.h"

extern uint16_t get_packet_size(struct obex_packet_t packet);

extern uint32_t get_connection_id(struct obex_packet_t packet);

extern size_t packet_to_str(struct obex_packet_t obex_packet, void* buf, size_t buflen);

extern size_t str_to_packet(struct obex_packet_t* obex_packet, const void* buf, size_t buflen);

extern uint8_t is_valid_header(uint8_t header_id);

extern enum HEADER_TYPES get_header_type(uint8_t header_id);

extern struct obex_packet_header_t* build_basic_header(struct obex_packet_header_t* headers, uint8_t header_id, uint32_t header_value);

extern struct obex_packet_header_t* build_extended_header(struct obex_packet_header_t* headers, uint8_t header_id, uint8_t* header_value, uint16_t value_size);

#if defined(__cplusplus)
}
#endif

#endif // __BLUETOOTH_OBEX_UTILS_H