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

#include "obex-utils.h"

/**
 * Gets the size of the packet.
 *
 * @param packet obex packet
 * @return size of the packet
 */
uint16_t get_packet_size(struct obex_packet_t packet)
{
	struct obex_packet_header_t* it;
	uint16_t packet_size;

	packet_size = (packet.info == NULL) ? (OBEX_MINIMUM_PACKET_SIZE) : (OBEX_MAXIMUM_PACKET_SIZE);

	it = packet.headers;
	while (it != NULL)
	{
		switch (it->header_type)
		{
			case BASIC_TYPE:
			{
				packet_size += sizeof(uint8_t) + sizeof(uint32_t);

				break;
			}
			case EXTENDED_TYPE:
			{
				packet_size += it->extended.header_size;

				break;
			}
			case UNKNOWN_TYPE:
			default:
			{
				break;
			}
		}
		it = it->next;
	}

	return packet_size;
}

/**
 * Gets the connection_id if the header exists.
 *
 * @param packet obex packet
 * @return connection_id if exists
 */
uint32_t get_connection_id(struct obex_packet_t packet)
{
	struct obex_packet_header_t* it;

	it = packet.headers;
	while (it != NULL)
	{
		if (it->header_id == OBEX_CONNECTION_ID)
		{
			return it->basic.header_value;
		}
		it = it->next;
	}

	return 0xFFFFFFFF;
}

/**
 * Gets the body if the header exists.
 *
 * @param packet obex packet
 * @return body if exists
 */
struct obex_packet_header_t* get_body_header(struct obex_packet_t packet)
{
	struct obex_packet_header_t* it;

	it = packet.headers;
	while (it != NULL)
	{
		if (it->header_id == OBEX_BODY)
		{
			return it;
		}
		it = it->next;
	}

	return NULL;
}

/**
 * Gets the body_end if the header exists.
 *
 * @param packet obex packet
 * @return body_end if exists
 */
struct obex_packet_header_t* get_body_end_header(struct obex_packet_t packet)
{
	struct obex_packet_header_t* it;

	it = packet.headers;
	while (it != NULL)
	{
		if (it->header_id == OBEX_BODY_END)
		{
			return it;
		}
		it = it->next;
	}

	return NULL;
}

/**
 * Dumps the packet into a string format.
 *
 * @param obex_packet obex packet
 * @param buf buf where the packet will be stored in string format
 * @param buflen size of the buf
 * @return number of bytes written, will be equals to the buflen
 */
size_t packet_to_str(struct obex_packet_t obex_packet, void* buf, size_t buflen)
{
	struct obex_packet_header_t* it;
	uint16_t offset;

	uint16_t be16_data;
	uint32_t be32_data;

	/* copy obex_packet_t.opcode */
	memcpy(buf, (const void*) &obex_packet.opcode, sizeof(uint8_t));
	/* copy packet_length in big endian format */
	be16_data = htobe16(obex_packet.packet_length);
	memcpy(((unsigned char*) buf) + OFFSET_PACKET_LENGTH, (const void*) &be16_data, sizeof(uint16_t));

	/* copy obex_packet_info_t if exists */
	if (obex_packet.info != NULL)
	{
		/* copy version and flags */
		memcpy(((unsigned char*) buf) + OFFSET_PACKET_INFO, (const void*) obex_packet.info, sizeof(uint8_t) + sizeof(uint8_t));
		/* copy maximum_packet_length in big endian format */
		be16_data = htobe16(obex_packet.info->maximum_packet_length);
		memcpy(((unsigned char*) buf) + OFFSET_PACKET_INFO + OFFSET_MAX_PACKET_LENGTH, (const void*) &be16_data, sizeof(uint16_t));
	}

	/* copy headers if exists */
	offset = (obex_packet.info == NULL) ? (OBEX_MINIMUM_PACKET_SIZE) : (OBEX_MAXIMUM_PACKET_SIZE);

	it = obex_packet.headers;
	while (it != NULL)
	{
		/* copy header_id */
		memcpy(((unsigned char*) buf) + offset, &it->header_id, sizeof(uint8_t));

		switch (it->header_type)
		{
			case BASIC_TYPE:
			{
				/* copy uint32_t header_value */
				be32_data = htobe32(it->basic.header_value);
				memcpy(((unsigned char*) buf) + offset + sizeof(uint8_t), &be32_data, sizeof(uint32_t));

				offset += sizeof(uint8_t) + sizeof(uint32_t);

				break;
			}
			case EXTENDED_TYPE:
			{
				/* copy uint16_t header_size */
				be16_data = htobe16(it->extended.header_size);
				memcpy(((unsigned char*) buf) + offset + sizeof(uint8_t), &be16_data, sizeof(uint16_t));

				/* copy uint8_t* header_value */
				memcpy(((unsigned char*) buf) + offset + sizeof(uint8_t) + sizeof(uint16_t), it->extended.header_value, it->extended.header_size - (sizeof(uint8_t) + sizeof(uint16_t)));

				offset += it->extended.header_size;

				break;
			}
			case UNKNOWN_TYPE:
			default:
			{
				break;
			}
		}
		it = it->next;
	}

	return buflen;
}

/**
 * Dumps the string into a packet format.
 *
 * @param obex_packet obex packet
 * @param buf buf where the packet is stored in string format
 * @param buflen size of the buf
 * @return number of bytes written, will be equals to the buflen
 */
size_t str_to_packet(struct obex_packet_t* obex_packet, const void* buf, size_t buflen)
{
	uint16_t offset;

	uint8_t header_id;

	uint32_t hdr32_value;

	uint16_t hdr16_size;
	uint8_t* hdr8_value;
	uint16_t value_size;

	/* copy opcode and packet_length */
	memcpy(obex_packet, buf, OBEX_MINIMUM_PACKET_SIZE);
	obex_packet->packet_length = be16toh(obex_packet->packet_length);
	assert(buflen == obex_packet->packet_length);

	obex_packet->info = NULL;
	obex_packet->headers = NULL;

	if (obex_packet->packet_length > OBEX_MINIMUM_PACKET_SIZE)
	{
		/* copy next byte to check if is an obex_packet_info_t or obex_packet_header_t */
		memcpy(&header_id, ((unsigned char*) buf) + OBEX_MINIMUM_PACKET_SIZE, sizeof(uint8_t));

		/* copy obex_packet_info_t if exists */
		if (! is_valid_header(header_id))
		{
			obex_packet->info = (struct obex_packet_info_t*) malloc(sizeof(struct obex_packet_info_t));
			assert(obex_packet->info != NULL);

			memcpy(obex_packet->info, ((unsigned char*) buf) + OFFSET_PACKET_INFO, sizeof(struct obex_packet_info_t));
			obex_packet->info->maximum_packet_length = be16toh(obex_packet->info->maximum_packet_length);
		}

		offset = (obex_packet->info == NULL) ? (OBEX_MINIMUM_PACKET_SIZE) : (OBEX_MAXIMUM_PACKET_SIZE);
		while (offset < obex_packet->packet_length)
		{
			/* copy header_id */
			memcpy(&header_id, ((unsigned char*) buf) + offset, sizeof(uint8_t));

			switch (get_header_type(header_id))
			{
				case BASIC_TYPE:
				{
					/* copy header_value */
					memcpy(&hdr32_value, ((unsigned char*) buf) + offset + sizeof(uint8_t), sizeof(uint32_t));
					hdr32_value = be32toh(hdr32_value);

					obex_packet->headers = build_basic_header(obex_packet->headers, header_id, hdr32_value);
					offset += sizeof(uint8_t) + sizeof(uint32_t);

					break;
				}
				case EXTENDED_TYPE:
				{
					/* copy header_size */
					memcpy(&hdr16_size, ((unsigned char*) buf) + offset + sizeof(uint8_t), sizeof(uint16_t));
					hdr16_size = be16toh(hdr16_size);

					/* copy header_value */
					value_size = hdr16_size - sizeof(uint8_t) - sizeof(uint16_t);
					hdr8_value = (uint8_t*) malloc(value_size * sizeof(uint8_t));
					assert(hdr8_value != NULL);

					memcpy(hdr8_value, ((unsigned char*) buf) + offset + sizeof(uint8_t) + sizeof(uint16_t), value_size);

					obex_packet->headers = build_extended_header(obex_packet->headers, header_id, hdr8_value, value_size);
					offset += sizeof(uint8_t) + sizeof(uint16_t) + value_size;

					free(hdr8_value);

					break;
				}
				case UNKNOWN_TYPE:
				default:
				{
					// error handling [...]
					offset = obex_packet->packet_length;

					break;
				}
			}
		}
	}


	return buflen;
}

/**
 * Checks if the header_id is a valid obex header.
 *
 * @param header_id identifier of the header
 * @return 1 if the header is valid, 0 otherwise
 */
uint8_t is_valid_header(uint8_t header_id)
{
	uint8_t is_valid;

	switch (header_id)
	{
		case OBEX_EMPTY:
		case OBEX_COUNT:
		case OBEX_NAME:
		case OBEX_TYPE:
		case OBEX_LENGTH:
		case OBEX_ISO_TIME:
		case OBEX_COMPATIBILITY_TIME:
		case OBEX_DESCRIPTION:
		case OBEX_TARGET:
		case OBEX_HTTP:
		case OBEX_BODY:
		case OBEX_BODY_END:
		case OBEX_WHO:
		case OBEX_CONNECTION_ID:
		case OBEX_APP_PARAMETERS:
		case OBEX_AUTH_CHALLENGE:
		case OBEX_AUTH_RESPONSE:
		case OBEX_CREATOR_ID:
		case OBEX_WAN_UUID:
		case OBEX_OBJECT_CLASS:
		case OBEX_SESSION_PARAMETERS:
		case OBEX_SESSION_SEQ_NUMBER:
		{
			is_valid = 1;

			break;
		}
		default:
		{
			is_valid = 0;

			break;
		}
	}

	return is_valid;
}

/**
 * Gets the type of the header identified by the header_id.
 *
 * @param header_id identifier of the header
 * @return basic or extended type
 */
enum HEADER_TYPES get_header_type(uint8_t header_id)
{
	enum HEADER_TYPES header_type;

	switch (header_id)
	{
		case OBEX_CONNECTION_ID:
		{
			header_type = BASIC_TYPE;

			break;
		}
		case OBEX_TARGET:
		case OBEX_WHO:
		case OBEX_NAME:
		case OBEX_TYPE:
		case OBEX_BODY:
		case OBEX_BODY_END:
		{
			header_type = EXTENDED_TYPE;

			break;
		}
		default:
		{
			fprintf(stderr, "[0x%02x] HEADER ID NOT ADDED!\n", header_id);
			header_type = UNKNOWN_TYPE;

			break;
		}
	}

	return header_type;
}

/**
 * Builds a basic obex header and adds it to the headers list.
 *
 * @param headers list of the headers, can be null
 * @param header_id identifier of the header
 * @param header_value value of the header
 * @return pointer to the headers list
 */
struct obex_packet_header_t* build_basic_header(struct obex_packet_header_t* headers, uint8_t header_id, uint32_t header_value)
{
	struct obex_packet_header_t* header;
	struct obex_packet_header_t* it;

	header = (struct obex_packet_header_t*) malloc(sizeof(struct obex_packet_header_t));
	assert(header != NULL);

	header->header_id = header_id;
	header->basic.header_value = header_value;

	header->header_type = BASIC_TYPE;
	header->next = NULL;

	if (headers != NULL)
	{
		it = headers;
		while (it->next != NULL)
		{
			it = it->next;
		}
		it->next = header;
	}

	return (headers != NULL) ? (headers) : (header);
}

/**
 * Builds an extended obex header and adds it to the headers list.
 *
 * @param headers list of the headers, can be null
 * @param header_id identifier of the header
 * @param header_value value of the header
 * @param value_size size of the value
 * @return pointer to the headers list
 */
struct obex_packet_header_t* build_extended_header(struct obex_packet_header_t* headers, uint8_t header_id, uint8_t* header_value, uint16_t value_size)
{
	struct obex_packet_header_t* header;
	struct obex_packet_header_t* it;

	header = (struct obex_packet_header_t*) malloc(sizeof(struct obex_packet_header_t));
	assert(header != NULL);

	header->header_id = header_id;
	header->extended.header_size = sizeof(uint8_t) + sizeof(uint16_t) + value_size;
	header->extended.header_value = (uint8_t*) malloc(value_size * sizeof(uint8_t));
	memcpy(header->extended.header_value, header_value, value_size);

	header->header_type = EXTENDED_TYPE;
	header->next = NULL;

	if (headers != NULL)
	{
		it = headers;
		while (it->next != NULL)
		{
			it = it->next;
		}
		it->next = header;
	}

	return (headers != NULL) ? (headers) : (header);
}

/**
 * Destroys the headers list freeing the memory.
 *
 * @param headers list of the headers
 */
void destroy_headers(struct obex_packet_header_t** headers)
{
	struct obex_packet_header_t* it;

	if (*headers != NULL)
	{
		it = *headers;
		while (it != NULL)
		{
			if ((*headers)->header_type == EXTENDED_TYPE)
			{
				free((*headers)->extended.header_value);
			}
			it = it->next;
			free(*headers);
			*headers = it;
		}
		*headers = NULL;
	}
}

/**
 * Destroys the packet freeing the memory.
 *
 * @param packet obex packet
 */
void destroy_packet(struct obex_packet_t** packet)
{
	if (*packet != NULL)
	{
		if ((*packet)->info != NULL)
		{
			free((*packet)->info);
		}
		if ((*packet)->headers != NULL)
		{
			destroy_headers(&(*packet)->headers);
		}
		free(*packet);
		*packet = NULL;
	}
}

/**
 * Displays the obex packet in a string format.
 *
 * @param packet obex packet
 */
void display_obex_packet(struct obex_packet_t packet)
{
	void* buf;
	size_t buflen;

	if (packet.packet_length > 0)
	{
		buf = malloc(packet.packet_length * sizeof(unsigned char));
		assert(buf != NULL);

		buflen = packet_to_str(packet, buf, packet.packet_length);
		display_obex_packet_str(buf, buflen);

		free(buf);
	}
}

/**
 * Displays the obex packet in a string format.
 *
 * @param buf buf where the packet is stored in string format
 * @param buflen size of the buf
 */
void display_obex_packet_str(const void* buf, size_t buflen)
{
	size_t i;

	if (buf != NULL && buflen > 0)
	{
		fprintf(stdout, "[0x%02zx] ", buflen);
		for (i = 0; i < buflen; i ++)
		{
			fprintf(stdout, "0x%02x ", ((unsigned char*) buf)[i]);
		}
		fprintf(stdout, "\n\n");
	}
}
