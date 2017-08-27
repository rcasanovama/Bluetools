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

#include "obex.h"

/**
 * Internal function to send the request to the OBEX server
 *
 * @param obex handler of the obex connection
 * @param request request packet
 * @return bytes sent
 */
static ssize_t internal_obex_send_packet(struct obex_t obex, struct obex_packet_t request)
{
	unsigned char* buf;
	size_t buflen;
	ssize_t send_len;

	buflen = request.packet_length * sizeof(unsigned char);
	buf = (unsigned char*) malloc(request.packet_length * sizeof(unsigned char));
	assert(buf != NULL);

	buflen = packet_to_str(request, buf, buflen);
	assert(buflen == (request.packet_length * sizeof(unsigned char)));

#ifndef NOVERBOSE
	display_obex_packet_str((const void*) buf, buflen);
#endif

	send_len = rfcomm_send(obex.rfcomm_socket, (const void*) buf, buflen);
	if (send_len < 0)
	{
		return - 1;
	}

	free(buf);

	return send_len;
}

/**
 * Internal function to receive the response from the OBEX server
 *
 * @param obex handler of the obex connection
 * @param response response packet
 * @return bytes received
 */
static ssize_t internal_obex_recv_packet(struct obex_t obex, struct obex_packet_t** response)
{
	unsigned char* buf;
	size_t buflen;
	ssize_t recv_len;

	buf = (unsigned char*) malloc((OBEX_CUSTOM_MTU + 1) * sizeof(unsigned char));
	assert(buf != NULL);

	buflen = OBEX_CUSTOM_MTU;

	recv_len = rfcomm_recv(obex.rfcomm_socket, (void*) buf, buflen);
	if (recv_len < 0)
	{
		*response = NULL;
		return - 1;
	}

	/* check if packets are too big */
	assert(recv_len < OBEX_CUSTOM_MTU + 1);

#ifndef NOVERBOSE
	display_obex_packet_str((const void*) buf, (size_t) recv_len);
#endif

	*response = (struct obex_packet_t*) malloc(sizeof(struct obex_packet_t));
	assert(*response != NULL);

	buflen = str_to_packet(*response, (const void*) buf, (size_t) recv_len);
	assert(buflen == (*response)->packet_length);

	free(buf);

	return recv_len;
}

/**
 * Starts an OBEX connection with the specified remote device
 *
 * @param dev_id identifier of the local bluetooth device
 * @param addr address of the remote device
 * @param channel channel of the remote OBEX service
 * @return handler of the obex connection
 */
struct obex_t obex_init(uint16_t dev_id, const char* addr, uint8_t channel)
{
	struct obex_t obex;

	obex.rfcomm_socket = rfcomm_client_socket(dev_id, addr, channel);

	obex.obex_status = (uint8_t) ((obex.rfcomm_socket.fd < 0) ? (OBEX_INIT_ERROR) : (OBEX_INIT_OK));
	obex.connection_id = 0xFFFFFFFF;

	return obex;
}

/**
 * Sends the connect action to remote device using the obex_init connection.
 *
 * @param obex handler of the obex connection
 * @param headers headers used in the obex_connect
 * @return response of the obex_connect
 */
struct obex_packet_t* obex_connect(struct obex_t* obex, struct obex_packet_header_t* headers)
{
	struct obex_packet_t request, * response;
	ssize_t send_len, recv_len;

	/* Request packet headers */
	request.headers = headers;

	/* Request packet info */
	request.info = (struct obex_packet_info_t*) malloc(sizeof(struct obex_packet_info_t));
	assert(request.info != NULL);

	request.info->version = 0x10;
	request.info->flags = 0x00;
	request.info->maximum_packet_length = OBEX_CUSTOM_MTU;

	/* Request packet */
	request.opcode = OBEX_CONNECT;
	request.packet_length = get_packet_size(request);

	send_len = internal_obex_send_packet(*obex, request);
	if (send_len < 0)
	{
		return NULL;
	}

	recv_len = internal_obex_recv_packet(*obex, &response);
	if (recv_len < 0)
	{
		return NULL;
	}

	obex->connection_id = get_connection_id(*response);
	if (obex->connection_id == 0xFFFFFFFF)
	{
		return NULL;
	}

	return response;
}

/**
 * Sends the get action to remote device using the obex_init connection.
 *
 * @param obex handler of the obex connection
 * @param headers headers used in the obex_get
 * @return response of the obex_get obex_get
 */
struct obex_packet_t* obex_get(struct obex_t* obex, struct obex_packet_header_t* headers)
{
	struct obex_packet_t request, * response;
	ssize_t send_len, recv_len;

	/* Request packet headers */
	request.headers = headers;

	/* Request packet info */
	request.info = NULL;

	/* Request packet */
	request.opcode = OBEX_GET;
	request.packet_length = get_packet_size(request);

	send_len = internal_obex_send_packet(*obex, request);
	if (send_len < 0)
	{
		return NULL;
	}

	recv_len = internal_obex_recv_packet(*obex, &response);
	if (recv_len < 0)
	{
		return NULL;
	}

	return response;
}

/**
 * Sends the get_final action to remote device using the obex_init connection.
 *
 * @param obex handler of the obex connection
 * @param headers headers used in the obex_get_final
 * @return response of the obex_get_final
 */
struct obex_packet_t* obex_get_final(struct obex_t* obex, struct obex_packet_header_t* headers)
{
	struct obex_packet_t request, * response;
	ssize_t send_len, recv_len;

	/* Request packet headers */
	request.headers = headers;

	/* Request packet info */
	request.info = NULL;

	/* Request packet */
	request.opcode = OBEX_GET_FINAL;
	request.packet_length = get_packet_size(request);

	send_len = internal_obex_send_packet(*obex, request);
	if (send_len < 0)
	{
		return NULL;
	}

	recv_len = internal_obex_recv_packet(*obex, &response);
	if (recv_len < 0)
	{
		return NULL;
	}

	return response;
}

/**
 * Sends the put action to remote device using the obex_init connection.
 *
 * @param obex handler of the obex connection
 * @param headers headers used in the obex_put
 * @return response of the obex_put
 */
struct obex_packet_t* obex_put(struct obex_t* obex, struct obex_packet_header_t* headers)
{
	struct obex_packet_t request, * response;
	ssize_t send_len, recv_len;

	/* Request packet headers */
	request.headers = headers;

	/* Request packet info */
	request.info = NULL;

	/* Request packet */
	request.opcode = OBEX_PUT;
	request.packet_length = get_packet_size(request);

	send_len = internal_obex_send_packet(*obex, request);
	if (send_len < 0)
	{
		return NULL;
	}

	recv_len = internal_obex_recv_packet(*obex, &response);
	if (recv_len < 0)
	{
		return NULL;
	}

	return response;
}

/**
 * Sends the put_final action to remote device using the obex_init connection.
 *
 * @param obex handler of the obex connection
 * @param headers headers used in the obex_put_final
 * @return response of the obex_put_final
 */
struct obex_packet_t* obex_put_final(struct obex_t* obex, struct obex_packet_header_t* headers)
{
	struct obex_packet_t request, * response;
	ssize_t send_len, recv_len;

	/* Request packet headers */
	request.headers = headers;

	/* Request packet info */
	request.info = NULL;

	/* Request packet */
	request.opcode = OBEX_PUT_FINAL;
	request.packet_length = get_packet_size(request);

	send_len = internal_obex_send_packet(*obex, request);
	if (send_len < 0)
	{
		return NULL;
	}

	recv_len = internal_obex_recv_packet(*obex, &response);
	if (recv_len < 0)
	{
		return NULL;
	}

	return response;
}

/**
 * Sends the disconnect action to remote device using the obex_init connection.
 *
 * @param obex handler of the obex connection
 * @param headers headers used in the obex_disconnect
 * @return response of the obex_disconnect
 */
struct obex_packet_t* obex_disconnect(struct obex_t* obex, struct obex_packet_header_t* headers)
{
	struct obex_packet_t request, * response;
	ssize_t send_len, recv_len;

	/* Request packet headers */
	request.headers = headers;

	/* Request packet info */
	request.info = NULL;

	/* Request packet */
	request.opcode = OBEX_DISCONNECT;
	request.packet_length = get_packet_size(request);

	send_len = internal_obex_send_packet(*obex, request);
	if (send_len < 0)
	{
		return NULL;
	}

	recv_len = internal_obex_recv_packet(*obex, &response);
	if (recv_len < 0)
	{
		return NULL;
	}

	return response;
}

/**
 * Cleans the OBEX connection
 *
 * @param obex handler of the obex connection
 */
void obex_cleanup(struct obex_t obex)
{
	rfcomm_cleanup(obex.rfcomm_socket);
}