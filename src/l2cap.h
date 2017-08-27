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

#ifndef __BLUETOOTH_L2CAP_H
#define __BLUETOOTH_L2CAP_H

#if defined(__cplusplus)
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>

struct l2cap_socket_t
{
	uint16_t dev_id;

	int32_t fd;
	int32_t rmt_fd;
};

/**
 * Starts a l2cap connection with the specified remote device.
 *
 * @param dev_id identifier of the local bluetooth device
 * @param addr address of the remote device
 * @param psm psm of the remote l2cap server
 * @return handler of the l2cap connection
 */
extern struct l2cap_socket_t l2cap_client_socket(uint16_t dev_id, const char* addr, uint8_t psm);

/**
 * Starts a l2cap server listening on the specified psm.
 *
 * @param dev_id identifier of the local bluetooth device
 * @param psm psm for listening incoming remote connections
 * @param connections number of incoming connection which will be accepted
 * @return handler of the l2cap connection
 */
extern struct l2cap_socket_t l2cap_server_socket(uint16_t dev_id, uint8_t psm, uint8_t connections);

/**
 * Sends a message to the remote device using the l2cap connection.
 *
 * @param l2cap_socket handler of the l2cap connection
 * @param buf buf where the message is stored
 * @param buflen size of the buf
 * @return number of bytes sent, will be equals to the buflen
 */
extern ssize_t l2cap_send(struct l2cap_socket_t l2cap_socket, const void* buf, size_t buflen);

/**
 * Receives a message from the remote device using the l2cap connection.
 *
 * @param l2cap_socket handler of the l2cap connection
 * @param buf buf where the message will be stored
 * @param buflen size of the buf
 * @return number of bytes received
 */
extern ssize_t l2cap_recv(struct l2cap_socket_t l2cap_socket, void* buf, size_t buflen);

/**
 * Cleans the l2cap connection.
 *
 * @param l2cap_socket handler of the l2cap connection
 */
extern void l2cap_cleanup(struct l2cap_socket_t l2cap_socket);

#if defined(__cplusplus)
}
#endif

#endif // __BLUETOOTH_L2CAP_H
