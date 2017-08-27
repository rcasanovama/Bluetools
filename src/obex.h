/**
 *
 *  Copyright (C) 2017  Raul Casanova Marques
 *
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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

/**
 * Starts an OBEX connection with the specified remote device.
 *
 * @param dev_id identifier of the local bluetooth device
 * @param addr address of the remote device
 * @param channel channel of the remote OBEX service
 * @return handler of the obex connection
 */
extern struct obex_t obex_init(uint16_t dev_id, const char* addr, uint8_t channel);

/**
 * Sends the connect action to remote device using the obex_init connection.
 *
 * @param obex handler of the obex connection
 * @param headers headers used in the obex_connect
 * @return response of the obex_connect
 */
extern struct obex_packet_t* obex_connect(struct obex_t* obex, struct obex_packet_header_t* headers);

/**
 * Sends the get action to remote device using the obex_init connection.
 *
 * @param obex handler of the obex connection
 * @param headers headers used in the obex_get
 * @return response of the obex_get obex_get
 */
extern struct obex_packet_t* obex_get(struct obex_t* obex, struct obex_packet_header_t* headers);

/**
 * Sends the get_final action to remote device using the obex_init connection.
 *
 * @param obex handler of the obex connection
 * @param headers headers used in the obex_get_final
 * @return response of the obex_get_final
 */
extern struct obex_packet_t* obex_get_final(struct obex_t* obex, struct obex_packet_header_t* headers);

/**
 * Sends the put action to remote device using the obex_init connection.
 *
 * @param obex handler of the obex connection
 * @param headers headers used in the obex_put
 * @return response of the obex_put
 */
extern struct obex_packet_t* obex_put(struct obex_t* obex, struct obex_packet_header_t* headers);

/**
 * Sends the put_final action to remote device using the obex_init connection.
 *
 * @param obex handler of the obex connection
 * @param headers headers used in the obex_put_final
 * @return response of the obex_put_final
 */
extern struct obex_packet_t* obex_put_final(struct obex_t* obex, struct obex_packet_header_t* headers);

/**
 * Sends the disconnect action to remote device using the obex_init connection.
 *
 * @param obex handler of the obex connection
 * @param headers headers used in the obex_disconnect
 * @return response of the obex_disconnect
 */
extern struct obex_packet_t* obex_disconnect(struct obex_t* obex, struct obex_packet_header_t* headers);

/**
 * Cleans the OBEX connection.
 *
 * @param obex handler of the obex connection
 */
extern void obex_cleanup(struct obex_t obex);

#if defined(__cplusplus)
}
#endif

#endif // __BLUETOOTH_OBEX_H