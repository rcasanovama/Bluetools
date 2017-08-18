#include "obex.h"

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

	send_len = rfcomm_send(obex.rfcomm_socket, (const void*) buf, buflen);
	if (send_len < 0)
	{
		return - 1;
	}

	return send_len;
}

static ssize_t internal_obex_recv_packet(struct obex_t obex, struct obex_packet_t** response)
{
	unsigned char* buf;
	size_t buflen;
	ssize_t recv_len;

	buf = (unsigned char*) malloc(OBEX_MAXIMUM_MTU * sizeof(unsigned char));
	assert(buf != NULL);

	buflen = OBEX_MAXIMUM_MTU;

	recv_len = rfcomm_recv(obex.rfcomm_socket, (void*) buf, buflen);
	if (recv_len < 0)
	{
		*response = NULL;
		return - 1;
	}

	/* check if packets are too big */
	assert(recv_len < OBEX_MAXIMUM_MTU);

	*response = (struct obex_packet_t*) malloc(sizeof(struct obex_packet_t));
	assert(*response != NULL);

	buflen = str_to_packet(*response, (const void*) buf, (size_t) recv_len);
	assert(buflen == (*response)->packet_length);

	return recv_len;
}

struct obex_t obex_init(uint16_t dev_id, const char* __addr, uint8_t __channel)
{
	struct obex_t obex;

	obex.rfcomm_socket = rfcomm_client_socket(dev_id, __addr, __channel);

	obex.obex_status = (uint8_t) ((obex.rfcomm_socket.fd < 0) ? (OBEX_INIT_ERROR) : (OBEX_INIT_OK));
	obex.connection_id = 0xFFFFFFFF;

	return obex;
}

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
	request.info->maximum_packet_length = OBEX_MAXIMUM_MTU;

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

struct obex_packet_t* obex_get(struct obex_t* obex, struct obex_packet_header_t* headers)
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

struct obex_packet_t* obex_put(struct obex_t* obex, struct obex_packet_header_t* headers)
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

int8_t obex_cleanup(struct obex_t obex)
{
	return rfcomm_cleanup(obex.rfcomm_socket);
}