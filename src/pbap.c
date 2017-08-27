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

#include "pbap.h"

/**
 * Starts a Phone Book Access Profile client, which enables devices to
 * exchange phone-book objects.
 *
 * @param dev_id identifier of the local bluetooth device
 * @param addr address of the remote device
 * @param channel channel of the remote PBAP service
 * @return handler of the pbap client
 */
struct pbap_t pbap_client(uint16_t dev_id, const char* addr, uint8_t channel)
{
	struct obex_packet_header_t* headers;
	struct obex_packet_t* packet;

	struct pbap_t pbap;

	pbap.obex = obex_init(dev_id, addr, channel);
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

/**
 * Cleans and disconnects the Phone Book Access Profile client
 *
 * @param pbap handler of the pbap client
 */
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