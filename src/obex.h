#ifndef __BLUETOOTH_OBEX_H
#define __BLUETOOTH_OBEX_H

#if defined(__cplusplus)
extern "C"
{
#endif

#include <stdint.h>
#include <endian.h>

#include "rfcomm.h"
#include "obex-types.h"

#define OBEX_INIT_OK		0
#define OBEX_INIT_ERROR		1

#define OBEX_SEND_OK		0
#define OBEX_SEND_ERROR		1

struct obex_t
{
	uint8_t obex_status;

	struct rfcomm_socket_t rfcomm_socket;
};

extern struct obex_t obex_init(uint16_t dev_id, const char* __addr, uint8_t __channel);

extern struct obex_packet_t obex_connect(struct obex_t __obex_t, struct obex_header_t* headers);

extern struct obex_packet_t obex_get(struct obex_t __obex_t, struct obex_header_t* headers);

extern struct obex_packet_t obex_put(struct obex_t __obex_t, struct obex_header_t* headers);

extern struct obex_packet_t obex_disconnect(struct obex_t __obex_t, struct obex_header_t* headers);

extern struct obex_header_t* obex_add_header(struct obex_header_t* headers, uint8_t header_id, uint8_t* header_value, uint16_t value_size);

extern int8_t obex_cleanup(struct obex_t __obex_t);


#if defined(__cplusplus)
}
#endif

#endif // __BLUETOOTH_OBEX_H