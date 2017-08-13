#include "obex.h"

static uint16_t internal_obex_packet_size(struct obex_header_t* headers)
{
	struct obex_header_t* iterator;
	uint16_t obex_packet_size;

	iterator = headers;
	obex_packet_size = OBEX_MINIMUM_PACKET_SIZE;

	while (iterator != NULL)
	{
		obex_packet_size += be16toh(iterator->header_size);
		iterator = iterator->next;
	}

	return obex_packet_size;
}

static uint8_t internal_obex_send_packet(struct obex_t __obex_t, struct obex_packet_t request)
{
	struct obex_header_t* iterator;
	uint8_t iterations;
	uint8_t offset;

	unsigned char* buf;
	ssize_t send_len;

	buf = (unsigned char*) calloc(1, be16toh(request.packet_length));
	memcpy((void*) buf, (const void*) &request, be16toh(request.packet_length));

	offset = OBEX_MINIMUM_PACKET_SIZE;
	iterator = request.headers;
	iterations = 0;

	while (iterator != NULL)
	{
		// copy header_id and header_size
		memcpy(&buf[offset], iterator, OBEX_MINIMUM_HEADER_SIZE);
		// copy header_value
		memcpy(&buf[offset + OBEX_MINIMUM_HEADER_SIZE], iterator->header_value, be16toh(iterator->header_size) - OBEX_MINIMUM_HEADER_SIZE);

		offset += be16toh(iterator->header_size);
		iterator = iterator->next;
		iterations ++;
	}

	printf("[0x%02x] ", be16toh(request.packet_length));
	for (int i = 0; i < be16toh(request.packet_length); i ++)
	{
		printf("0x%02x ", buf[i]);
	}
	printf("\n");

	send_len = rfcomm_send(__obex_t.rfcomm_socket, (const void*) buf, be16toh(request.packet_length));
	if (send_len < 0)
	{
		return OBEX_SEND_ERROR;
	}

	return OBEX_SEND_OK;
}

struct obex_t obex_init(uint16_t dev_id, const char* __addr, uint8_t __channel)
{
	struct obex_t obex;

	obex.rfcomm_socket = rfcomm_client_socket(dev_id, __addr, __channel);
	if (obex.rfcomm_socket.fd < 0)
	{
		obex.obex_status = OBEX_INIT_ERROR;
	}
	else
	{
		obex.obex_status = OBEX_INIT_OK;
	}

	return obex;
}

struct obex_packet_t obex_connect(struct obex_t __obex_t, struct obex_header_t* headers)
{
	struct obex_packet_t request, response;

	/* Request packet */
	request.opcode = OBEX_CONNECT;
	request.packet_length = htobe16(internal_obex_packet_size(headers));
	request.version = 0x10;
	request.flags = 0x00;
	request.maximum_packet_length = htobe16(OBEX_MAXIMUM_MTU);
	request.headers = headers;

	if (internal_obex_send_packet(__obex_t, request) == OBEX_SEND_OK)
	{

	}
}

struct obex_packet_t obex_disconnect(struct obex_t __obex_t, struct obex_header_t* headers)
{
	struct obex_packet_t request, response;

	/* Request packet */
	request.opcode = OBEX_DISCONNECT;
	request.packet_length = htobe16(internal_obex_packet_size(headers));
	request.version = 0x10;
	request.flags = 0x00;
	request.maximum_packet_length = htobe16(OBEX_MAXIMUM_MTU);
	request.headers = headers;

	if (internal_obex_send_packet(__obex_t, request) == OBEX_SEND_OK)
	{

	}
}

struct obex_header_t* obex_add_header(struct obex_header_t* headers, uint8_t header_id, uint8_t* header_value, uint16_t value_size)
{
	struct obex_header_t* header;
	struct obex_header_t* src;

	header = (struct obex_header_t*) malloc(sizeof(struct obex_header_t));
	if (header == NULL)
	{
#ifndef NOVERBOSE
		perror("add_header");
#endif
		return NULL;
	}

	header->header_id = header_id;
	header->header_size = htobe16(sizeof(uint8_t) + value_size + sizeof(uint16_t));
	header->header_value = header_value;
	header->next = NULL;

	if (headers != NULL)
	{
		src = headers;
		while (headers->next != NULL)
		{
			headers = headers->next;
		}
		headers->next = header;
		headers = src;
	}

	return (headers != NULL) ? headers : header;
}
