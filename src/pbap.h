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

/**
 * Starts a Phone Book Access Profile client, which enables devices to
 * exchange phone-book objects.
 *
 * @param dev_id identifier of the local bluetooth device
 * @param addr address of the remote device
 * @param channel channel of the remote PBAP service
 * @return handler of the pbap client
 */
extern struct pbap_t pbap_client(uint16_t dev_id, const char* addr, uint8_t channel);

/**
 * Gets a phone-book object identified by the name using the PBAP client.
 *
 * @param pbap handler of the pbap client
 * @param name name of the remote phone-book object
 * @param name_size size of the name
 * @param type type of the remote phone-book object
 * @param type_size size of the type
 * @param buf buf where the retrieved data will be stored, should be null
 * @return the size of the retrieved data and the size of buf
 */
extern size_t pbap_get(struct pbap_t pbap, uint8_t* name, uint16_t name_size, uint8_t* type, uint16_t type_size, void** buf);

/**
 * Cleans and disconnects the Phone Book Access Profile client.
 *
 * @param pbap handler of the pbap client
 */
extern void pbap_cleanup(struct pbap_t pbap);

#if defined(__cplusplus)
}
#endif

#endif // __BLUETOOTH_PBAP_H
