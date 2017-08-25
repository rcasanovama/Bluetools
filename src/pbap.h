#ifndef __BLUETOOTH_PBAP_H
#define __BLUETOOTH_PBAP_H

#if defined(__cplusplus)
extern "C"
{
#endif

#include <stdint.h>

#include "obex-types.h"
#include "obex-utils.h"
#include "obex.h"

#include "pbap-types.h"

#define PBAP_INIT_OK        OBEX_INIT_OK
#define PBAP_INIT_ERROR     OBEX_INIT_ERROR

struct pbap_t
{
	uint8_t pbap_status;

	struct obex_t obex;
};

extern struct pbap_t pbap_client(uint16_t dev_id, const char* __addr, uint8_t __channel);

extern size_t pbap_get(struct pbap_t pbap, uint8_t* name, uint16_t name_size, uint8_t* type, uint16_t type_size, void** buf);

extern void pbap_cleanup(struct pbap_t pbap);

#if defined(__cplusplus)
}
#endif

#endif // __BLUETOOTH_PBAP_H
