#include "pbap.h"

struct pbap_t pbap_client(uint16_t dev_id, const char* __addr, uint8_t __channel)
{
	struct obex_packet_header_t* headers;
	struct obex_packet_t* packet;

	struct pbap_t pbap;

	pbap.obex = obex_init(dev_id, __addr, __channel);
	if (pbap.obex.obex_status == OBEX_INIT_ERROR)
	{
		pbap.pbap_status = PBAP_INIT_ERROR;
		return pbap;
	}

	headers = build_extended_header(NULL, OBEX_TARGET, UUID_PBAP, sizeof(UUID_PBAP));
	packet = obex_connect(&pbap.obex, headers);

	/* check packet status */
	pbap.pbap_status = (uint8_t) ((packet == NULL || packet->opcode != OBEX_SUCCESS) ? (PBAP_INIT_ERROR) : (PBAP_INIT_OK));

	/* clean headers and packet */
	destroy_headers(&headers);
	destroy_packet(&packet);

	return pbap;
}

size_t pbap_get(struct pbap_t pbap, uint8_t* name, uint16_t name_size, uint8_t* type, uint16_t type_size, void** buf)
{
	struct obex_packet_header_t* headers, * body_header;
	struct obex_packet_t* packet;
	size_t buflen, offset;

	headers = build_basic_header(NULL, OBEX_CONNECTION_ID, pbap.obex.connection_id);
	headers = build_extended_header(headers, OBEX_NAME, name, name_size);
	headers = build_extended_header(headers, OBEX_TYPE, type, type_size);

	*buf = malloc(sizeof(unsigned char));
	assert(*buf != NULL);
	buflen = 0;

	/* client get */
	packet = obex_get_final(&pbap.obex, headers);
	destroy_headers(&headers);

	/* server responds 0x90 continue */
	while (packet->opcode == OBEX_CONTINUE)
	{
		/* read body_header */
		body_header = get_body_header(*packet);
		if (body_header != NULL)
		{
			/* offset */
			offset = buflen;
			/* offset + size of new block */
			buflen += body_header->extended.header_size - (sizeof(uint8_t) + sizeof(uint16_t));
			*buf = realloc(*buf, buflen * sizeof(unsigned char));
			/* copy the new block */
			memcpy(((unsigned char*) *buf) + offset, body_header->extended.header_value, body_header->extended.header_size - (sizeof(uint8_t) + sizeof(uint16_t)));
		}
		destroy_packet(&packet);

		/* get next chunk */
		packet = obex_get_final(&pbap.obex, NULL);
	}

	/* last chunk with opcode OBEX_SUCCESS */
	body_header = get_body_end_header(*packet);

	/* offset */
	offset = buflen;
	/* offset + size of new block */
	buflen += body_header->extended.header_size - (sizeof(uint8_t) + sizeof(uint16_t));
	*buf = realloc(*buf, buflen * sizeof(unsigned char));
	/* copy the new block */
	memcpy(((unsigned char*) *buf) + offset, body_header->extended.header_value, body_header->extended.header_size - (sizeof(uint8_t) + sizeof(uint16_t)));

	destroy_packet(&packet);

	return buflen;
}

void pbap_cleanup(struct pbap_t pbap)
{
	struct obex_packet_header_t* headers;
	struct obex_packet_t* packet;

	headers = build_basic_header(NULL, OBEX_CONNECTION_ID, pbap.obex.connection_id);
	packet = obex_disconnect(&pbap.obex, headers);
	assert(packet->opcode == OBEX_SUCCESS);

	destroy_headers(&headers);
	destroy_packet(&packet);

	obex_cleanup(pbap.obex);
}