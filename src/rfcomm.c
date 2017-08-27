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

#include "rfcomm.h"

/**
 * Internal function to clean the local rfcomm socket.
 *
 * @param rfcomm_socket handler of the rfcomm connection
 */
static void internal_rfcomm_cleanup_fd(struct rfcomm_socket_t rfcomm_socket)
{
	int r;

	r = shutdown(rfcomm_socket.fd, SHUT_RDWR);
#ifndef NOVERBOSE
	if (r < 0)
	{
		perror("rfcomm_shutdown_fd");
	}
#endif

	r = close(rfcomm_socket.fd);
#ifndef NOVERBOSE
	if (r < 0)
	{
		perror("rfcomm_close_fd");
	}
#endif
}

/**
 * Internal function to connect to remote device.
 *
 * @param rfcomm_socket handler of the rfcomm connection
 * @param addr address of the remote device
 * @param channel channel of the remote rfcomm server
 * @return status code, -1 if error
 */
static int32_t internal_rfcomm_connect(struct rfcomm_socket_t rfcomm_socket, const char* addr, uint8_t channel)
{
	struct sockaddr_rc sockaddr;
	int r;

	sockaddr.rc_family = AF_BLUETOOTH;
	sockaddr.rc_channel = channel;
	str2ba(addr, &sockaddr.rc_bdaddr);

	r = connect(rfcomm_socket.fd, (struct sockaddr*) &sockaddr, sizeof(sockaddr));
#ifndef NOVERBOSE
	if (r < 0)
	{
		perror("rfcomm_connect");
	}
#endif

	return r;
}

/**
 * Internal function to accept incoming remote connections.
 *
 * @param rfcomm_socket handler of the rfcomm connection
 * @param connections number of incoming connection which will be accepted
 * @return file descriptor of the remote connection
 */
static int32_t internal_rfcomm_accept(struct rfcomm_socket_t rfcomm_socket, uint8_t connections)
{
	struct sockaddr_rc addr;
	socklen_t addrlen;
	int r;

	r = listen(rfcomm_socket.fd, (connections == 0) ? (1) : (connections));
	if (r < 0)
	{
#ifndef NOVERBOSE
		perror("rfcomm_listen");
#endif
		return r;
	}

	r = accept(rfcomm_socket.fd, (struct sockaddr*) &addr, &addrlen);
#ifndef NOVERBOSE
	if (r < 0)
	{
		perror("rfcomm_accept");
	}
#endif

	return r;

}

/**
 * Internal function to bind the local address to a channel.
 *
 * @param rfcomm_socket handler of the rfcomm connection
 * @param channel channel for listening incoming remote connections
 * @return status code, -1 if error
 */
static int8_t internal_rfcomm_bind(struct rfcomm_socket_t rfcomm_socket, uint8_t channel)
{
	struct hci_dev_info dev_info;
	struct sockaddr_rc addr;
	int r;

	if (rfcomm_socket.fd < 0)
	{
		return - 1;
	}

	if (hci_devinfo(rfcomm_socket.dev_id, &dev_info) < 0)
	{
		perror("hci_devinfo");
		return - 1;
	}

	memset(&addr, 0, sizeof(addr));

	addr.rc_family = AF_BLUETOOTH;
	addr.rc_channel = channel;
	addr.rc_bdaddr = dev_info.bdaddr;

	r = bind(rfcomm_socket.fd, (struct sockaddr*) &addr, sizeof(addr));
#ifndef NOVERBOSE
	if (r < 0)
	{
		perror("rfcomm_bind");
	}
#endif

	return r;
}

/**
 * Internal function to create a rfcomm socket.
 *
 * @return handler of the rfcomm connection
 */
static struct rfcomm_socket_t internal_rfcomm_socket()
{
	struct rfcomm_socket_t rfcomm_socket;

	memset(&rfcomm_socket, 0, sizeof(rfcomm_socket));

	rfcomm_socket.fd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
#ifndef NOVERBOSE
	if (rfcomm_socket.fd < 0)
	{
		perror("rfcomm_socket");
	}
#endif

	return rfcomm_socket;
}

/**
 * Starts a rfcomm connection with the specified remote device.
 *
 * @param dev_id identifier of the local bluetooth device
 * @param addr address of the remote device
 * @param channel channel of the remote rfcomm server
 * @return handler of the rfcomm connection
 */
struct rfcomm_socket_t rfcomm_client_socket(uint16_t dev_id, const char* addr, uint8_t channel)
{
	struct rfcomm_socket_t rfcomm_socket;
	int r;

	rfcomm_socket = internal_rfcomm_socket();
	if (rfcomm_socket.fd < 0)
	{
		return rfcomm_socket;
	}

	rfcomm_socket.dev_id = dev_id;

	r = internal_rfcomm_connect(rfcomm_socket, addr, channel);
	if (r < 0)
	{
		internal_rfcomm_cleanup_fd(rfcomm_socket);
		rfcomm_socket.fd = - 1;

		return rfcomm_socket;
	}

	rfcomm_socket.rmt_fd = rfcomm_socket.fd;

	return rfcomm_socket;
}

/**
 * Starts a rfcomm server listening on the specified channel.
 *
 * @param dev_id identifier of the local bluetooth device
 * @param channel channel for listening incoming remote connections
 * @param connections number of incoming connection which will be accepted
 * @return handler of the rfcomm connection
 */
struct rfcomm_socket_t rfcomm_server_socket(uint16_t dev_id, uint8_t channel, uint8_t connections)
{
	struct rfcomm_socket_t rfcomm_socket;
	int r;

	rfcomm_socket = internal_rfcomm_socket();
	if (rfcomm_socket.fd < 0)
	{
		return rfcomm_socket;
	}

	rfcomm_socket.dev_id = dev_id;

	r = internal_rfcomm_bind(rfcomm_socket, channel);
	if (r < 0)
	{
		internal_rfcomm_cleanup_fd(rfcomm_socket);
		rfcomm_socket.fd = - 1;

		return rfcomm_socket;
	}

	rfcomm_socket.rmt_fd = internal_rfcomm_accept(rfcomm_socket, connections);
	if (rfcomm_socket.rmt_fd < 0)
	{
		internal_rfcomm_cleanup_fd(rfcomm_socket);
		rfcomm_socket.fd = - 1;

		return rfcomm_socket;
	}

	return rfcomm_socket;
}

/**
 * Sends a message to the remote device using the rfcomm connection.
 *
 * @param rfcomm_socket handler of the rfcomm connection
 * @param buf buf where the message is stored
 * @param buflen size of the buf
 * @return number of bytes sent, will be equals to the buflen
 */
ssize_t rfcomm_send(struct rfcomm_socket_t rfcomm_socket, const void* buf, size_t buflen)
{
	ssize_t send_len;

	send_len = send(rfcomm_socket.rmt_fd, buf, buflen, 0);
#ifndef NOVERBOSE
	if (send_len < 0)
	{
		perror("rfcomm_send");
	}
#endif

	return send_len;
}

/**
 * Receives a message from the remote device using the rfcomm connection.
 *
 * @param rfcomm_socket handler of the rfcomm connection
 * @param buf buf where the message will be stored
 * @param buflen size of the buf
 * @return number of bytes received
 */
ssize_t rfcomm_recv(struct rfcomm_socket_t rfcomm_socket, void* buf, size_t buflen)
{
	ssize_t recv_len;

	recv_len = recv(rfcomm_socket.rmt_fd, buf, buflen, 0);
#ifndef NOVERBOSE
	if (recv_len < 0)
	{
		perror("rfcomm_recv");
	}
#endif

	return recv_len;
}

/**
 * Cleans the rfcomm connection.
 *
 * @param rfcomm_socket handler of the rfcomm connection
 */
void rfcomm_cleanup(struct rfcomm_socket_t rfcomm_socket)
{
	int r;

	if (rfcomm_socket.rmt_fd != rfcomm_socket.fd)
	{
		r = shutdown(rfcomm_socket.rmt_fd, SHUT_RDWR);
#ifndef NOVERBOSE
		if (r < 0)
		{
			perror("rfcomm shutdown rfd");
		}
#endif

		r = close(rfcomm_socket.rmt_fd);
#ifndef NOVERBOSE
		if (r < 0)
		{
			perror("rfcomm close rfd");
		}
#endif
	}

	r = shutdown(rfcomm_socket.fd, SHUT_RDWR);
#ifndef NOVERBOSE
	if (r < 0)
	{
		perror("rfcomm shutdown fd");
	}
#endif

	r = close(rfcomm_socket.fd);
#ifndef NOVERBOSE
	if (r < 0)
	{
		perror("rfcomm close fd");
	}
#endif
}