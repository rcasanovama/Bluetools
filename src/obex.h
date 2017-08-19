#ifndef __BLUETOOTH_OBEX_H
#define __BLUETOOTH_OBEX_H

#if defined(__cplusplus)
extern "C"
{
#endif

#include <stdint.h>
#include <assert.h>

#include "rfcomm.h"
#include "obex-types.h"
#include "obex-utils.h"

#define OBEX_INIT_OK        0
#define OBEX_INIT_ERROR     1

struct obex_t
{
	uint8_t obex_status;
	uint32_t connection_id;

	struct rfcomm_socket_t rfcomm_socket;
};

extern struct obex_t obex_init(uint16_t dev_id, const char* __addr, uint8_t __channel);

extern struct obex_packet_t* obex_connect(struct obex_t* obex, struct obex_packet_header_t* headers);

extern struct obex_packet_t* obex_get(struct obex_t* obex, struct obex_packet_header_t* headers);

extern struct obex_packet_t* obex_get_final(struct obex_t* obex, struct obex_packet_header_t* headers);

extern struct obex_packet_t* obex_put(struct obex_t* obex, struct obex_packet_header_t* headers);

extern struct obex_packet_t* obex_put_final(struct obex_t* obex, struct obex_packet_header_t* headers);

extern struct obex_packet_t* obex_disconnect(struct obex_t* obex, struct obex_packet_header_t* headers);

extern int8_t obex_cleanup(struct obex_t obex);

#if defined(__cplusplus)
}
#endif

#endif // __BLUETOOTH_OBEX_H