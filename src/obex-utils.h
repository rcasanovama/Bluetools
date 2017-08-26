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

/**
 * Gets the size of the packet.
 *
 * @param packet obex packet
 * @return size of the packet
 */
extern uint16_t get_packet_size(struct obex_packet_t packet);

/**
 * Gets the connection_id if the header exists.
 *
 * @param packet obex packet
 * @return connection_id if exists
 */
extern uint32_t get_connection_id(struct obex_packet_t packet);

/**
 * Gets the body if the header exists.
 *
 * @param packet obex packet
 * @return body if exists
 */
extern struct obex_packet_header_t* get_body_header(struct obex_packet_t packet);

/**
 * Gets the body_end if the header exists.
 *
 * @param packet obex packet
 * @return body_end if exists
 */
extern struct obex_packet_header_t* get_body_end_header(struct obex_packet_t packet);

/**
 * Dumps the packet into a string format.
 *
 * @param obex_packet obex packet
 * @param buf buf where the packet will be stored in string format
 * @param buflen size of the buf
 * @return number of bytes written, will be equals to the buflen
 */
extern size_t packet_to_str(struct obex_packet_t obex_packet, void* buf, size_t buflen);

/**
 * Dumps the string into a packet format.
 *
 * @param obex_packet obex packet
 * @param buf buf where the packet is stored in string format
 * @param buflen size of the buf
 * @return number of bytes written, will be equals to the buflen
 */
extern size_t str_to_packet(struct obex_packet_t* obex_packet, const void* buf, size_t buflen);

/**
 * Checks if the header_id is a valid obex header.
 *
 * @param header_id identifier of the header
 * @return 1 if the header is valid, 0 otherwise
 */
extern uint8_t is_valid_header(uint8_t header_id);

/**
 * Gets the type of the header identified by the header_id.
 *
 * @param header_id identifier of the header
 * @return basic or extended type
 */
extern enum HEADER_TYPES get_header_type(uint8_t header_id);

/**
 * Builds a basic obex header and adds it to the headers list.
 *
 * @param headers list of the headers, can be null
 * @param header_id identifier of the header
 * @param header_value value of the header
 * @return pointer to the headers list
 */
extern struct obex_packet_header_t* build_basic_header(struct obex_packet_header_t* headers, uint8_t header_id, uint32_t header_value);

/**
 * Builds an extended obex header and adds it to the headers list.
 *
 * @param headers list of the headers, can be null
 * @param header_id identifier of the header
 * @param header_value value of the header
 * @param value_size size of the value
 * @return pointer to the headers list
 */
extern struct obex_packet_header_t* build_extended_header(struct obex_packet_header_t* headers, uint8_t header_id, uint8_t* header_value, uint16_t value_size);

/**
 * Destroys the headers list freeing the memory.
 *
 * @param headers list of the headers
 */
extern void destroy_headers(struct obex_packet_header_t** headers);

/**
 * Destroys the packet freeing the memory.
 *
 * @param packet obex packet
 */
extern void destroy_packet(struct obex_packet_t** packet);

/**
 * Displays the obex packet in a string format.
 *
 * @param packet obex packet
 */
extern void display_obex_packet(struct obex_packet_t packet);

/**
 * Displays the obex packet in a string format.
 *
 * @param buf buf where the packet is stored in string format
 * @param buflen size of the buf
 */
extern void display_obex_packet_str(const void* buf, size_t buflen);

#if defined(__cplusplus)
}
#endif

#endif // __BLUETOOTH_OBEX_UTILS_H