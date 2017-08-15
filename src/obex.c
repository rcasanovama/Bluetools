#include "obex.h"

static uint16_t internal_obex_packet_size(struct obex_packet_info_t* info, struct obex_packet_header_t* headers)
{
	struct obex_packet_header_t* iterator;
	uint16_t obex_packet_size;

	iterator = headers;
	obex_packet_size = (info == NULL) ? (OBEX_MINIMUM_PACKET_SIZE) : (OBEX_MAXIMUM_PACKET_SIZE);

	while (iterator != NULL)
	{
		obex_packet_size += iterator->header_size;
		iterator = iterator->next;
	}

	return obex_packet_size;
}

static ssize_t internal_obex_send_packet(struct obex_t __obex_t, struct obex_packet_t request)
{
	struct obex_packet_header_t* iterator;
	uint8_t iterations;
	uint8_t offset;

	uint16_t big_endian_data;
	unsigned char* buf;
	ssize_t send_len;

	buf = (unsigned char*) malloc(request.packet_length * sizeof(unsigned char));
	assert(buf != NULL);

	// copy opcode
	memcpy((void*) buf, (const void*) &request, sizeof(uint8_t));
	// copy packet_length in big endian format
	big_endian_data = htobe16(request.packet_length);
	memcpy((void*) &buf[OFFSET_PACKET_LENGTH], (const void*) &big_endian_data, sizeof(uint16_t));

	// copy obex_packet_info_t if exists
	if (request.info != NULL)
	{
		memcpy((void*) &buf[OBEX_MINIMUM_PACKET_SIZE], (const void*) request.info, OFFSET_MAX_PACKET_LENGTH);
		// copy maximum_packet_length in big endian format
		big_endian_data = htobe16(request.info->maximum_packet_length);
		memcpy((void*) &buf[OBEX_MINIMUM_PACKET_SIZE + OFFSET_MAX_PACKET_LENGTH], (const void*) &big_endian_data, sizeof(uint16_t));
	}

	offset = (request.info == NULL) ? (OBEX_MINIMUM_PACKET_SIZE) : (OBEX_MAXIMUM_PACKET_SIZE);
	iterator = request.headers;
	iterations = 0;

	while (iterator != NULL)
	{
		// copy header_id
		memcpy(&buf[offset], &iterator->header_id, sizeof(uint8_t));
		// copy header_size in big endian format
		big_endian_data = htobe16(iterator->header_size);
		memcpy(&buf[offset + OFFSET_HEADER_LENGTH], &big_endian_data, sizeof(uint16_t));
		// copy header_value
		memcpy(&buf[offset + OBEX_MINIMUM_HEADER_SIZE], iterator->header_value, iterator->header_size - OBEX_MINIMUM_HEADER_SIZE);

		offset += iterator->header_size;
		iterator = iterator->next;
		iterations ++;
	}

//	printf("[0x%02x] ", request.packet_length);
//	for (int i = 0; i < request.packet_length; i ++)
//	{
//		printf("0x%02x ", buf[i]);
//	}
//	printf("\n");

	send_len = rfcomm_send(__obex_t.rfcomm_socket, (const void*) buf, request.packet_length);
	if (send_len < 0)
	{
		return OBEX_SEND_ERROR;
	}

	return OBEX_SEND_OK;
}

static struct obex_packet_t internal_obex_recv_packet(struct obex_t __obex_t, uint8_t operation)
{
	struct obex_packet_t response;
	ssize_t recv_len;

	unsigned char* buf;
	size_t buflen;

	struct obex_packet_header_t* iterator;
	uint8_t offset;

	uint8_t header_id;
	uint16_t header_size;
	uint8_t* header_value;
	uint16_t value_size;

	buf = (unsigned char*) malloc(OBEX_MAXIMUM_MTU * sizeof(unsigned char));
	assert(buf != NULL);

	buflen = OBEX_MAXIMUM_MTU;

	recv_len = rfcomm_recv(__obex_t.rfcomm_socket, (void*) buf, buflen);
	if (recv_len < 0)
	{
//		return OBEX_SEND_ERROR;
	}

	memcpy(&response, buf, OBEX_MINIMUM_PACKET_SIZE);
	// big endian to host endian
	response.packet_length = be16toh(response.packet_length);

	switch (operation)
	{
		case OBEX_CONNECT:
		{
			response.info = (struct obex_packet_info_t*) malloc(sizeof(struct obex_packet_info_t));
			assert(response.info != NULL);

			memcpy(response.info, &buf[OBEX_MINIMUM_PACKET_SIZE], sizeof(struct obex_packet_info_t));
			// big endian to host endian
			response.info->maximum_packet_length = be16toh(response.info->maximum_packet_length);

			break;
		}
		default:
		{
			response.info = NULL;

			break;
		}
	}

	offset = (response.info == NULL) ? (OBEX_MINIMUM_PACKET_SIZE) : (OBEX_MAXIMUM_PACKET_SIZE);
	while(offset < response.packet_length)
	{
		// copy header_id
		memcpy(&header_id, &buf[offset], sizeof(uint8_t));
		printf("header id  : 0x%02x\n", header_id);

		// copy header_size
		memcpy(&header_size, &buf[offset + OFFSET_HEADER_LENGTH], sizeof(uint16_t));
		// big endian to host endian
		header_size = be16toh(header_size);
		printf("header size: 0x%02x\n", header_size);

		// copy header_value
		value_size = header_size - sizeof(uint8_t) - sizeof(uint16_t);
		header_value = (uint8_t*) malloc(value_size * sizeof(uint8_t));
		assert(header_value != NULL);
		memcpy(header_value, &buf[offset + OFFSET_HEADER_VALUE], sizeof(uint8_t));

		// create header
//		response.headers = obex_add_header(response.headers, header_id, header_value, value_size);
		offset += header_size;
	}

//	response.headers =
	printf("opcode : 0x%02x\n", response.opcode);
	printf("length : 0x%02x\n", response.packet_length);
	printf("version: 0x%02x\n", response.info->version);
	printf("flags  : 0x%02x\n", response.info->flags);
	printf("max len: 0x%02x\n", response.info->maximum_packet_length);

	for (int i = 0; i < recv_len; i ++)
	{
		printf("0x%02x ", buf[i]);
	}
	printf("\n");

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

struct obex_packet_t obex_connect(struct obex_t __obex_t, struct obex_packet_header_t* headers)
{
	struct obex_packet_t request, response;

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
	request.packet_length = internal_obex_packet_size(request.info, request.headers);

	if (internal_obex_send_packet(__obex_t, request) == OBEX_SEND_OK)
	{
		return internal_obex_recv_packet(__obex_t, OBEX_CONNECT);
	}
}

struct obex_packet_t obex_disconnect(struct obex_t __obex_t, struct obex_packet_header_t* headers)
{
	struct obex_packet_t request, response;

	/* Request packet */
//	request.opcode = OBEX_DISCONNECT;
//	request.packet_length = htobe16(internal_obex_packet_size(headers));
//	request.version = 0x10;
//	request.flags = 0x00;
//	request.maximum_packet_length = htobe16(OBEX_MAXIMUM_MTU);
//	request.headers = headers;
//
//	if (internal_obex_send_packet(__obex_t, request) == OBEX_SEND_OK)
//	{
//
//	}
}

struct obex_packet_header_t* obex_add_header(struct obex_packet_header_t* headers, uint8_t header_id, uint8_t* header_value, uint16_t value_size)
{
	struct obex_packet_header_t* header;
	struct obex_packet_header_t* src;

	header = (struct obex_packet_header_t*) malloc(sizeof(struct obex_packet_header_t));
	if (header == NULL)
	{
#ifndef NOVERBOSE
		perror("add_header");
#endif
		return NULL;
	}

	header->header_id = header_id;
	switch (header_id)
	{
		case OBEX_CONNECTION_ID:
		{
			header->connection_id =
			break;
		}
		default:
		{
			header->header_size = sizeof(uint8_t) + value_size + sizeof(uint16_t);
			header->header_value = header_value;

			break;
		}
	}
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
