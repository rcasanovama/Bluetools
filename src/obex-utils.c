#include "obex-utils.h"

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
				packet_size += packet.headers->extended.header_size;

				break;
			}
			default:
			{
				break;
			}
		}
		it = it->next;
	}

	return packet_size;
}

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
	memcpy(buf + OFFSET_PACKET_LENGTH, (const void*) &be16_data, sizeof(uint16_t));

	/* copy obex_packet_info_t if exists */
	if (obex_packet.info != NULL)
	{
		/* copy version and flags */
		memcpy(buf + OFFSET_PACKET_INFO, (const void*) &obex_packet.info, sizeof(uint8_t) + sizeof(uint8_t));
		/* copy maximum_packet_length in big endian format */
		be16_data = htobe16(obex_packet.info->maximum_packet_length);
		memcpy(buf + OFFSET_PACKET_INFO + OFFSET_MAX_PACKET_LENGTH, (const void*) &be16_data, sizeof(uint16_t));
	}

	/* copy headers if exists */
	offset = (obex_packet.info == NULL) ? (OBEX_MINIMUM_PACKET_SIZE) : (OBEX_MAXIMUM_PACKET_SIZE);

	it = obex_packet.headers;
	while (it != NULL)
	{
		/* copy header_id */
		memcpy(buf + offset, &it->header_id, sizeof(uint8_t));

		switch (it->header_type)
		{
			case BASIC_TYPE:
			{
				/* copy uint32_t header_value */
				be32_data = htobe32(it->basic.header_value);
				memcpy(buf + offset + sizeof(uint8_t), &be32_data, sizeof(uint32_t));

				offset += sizeof(uint8_t) + sizeof(uint32_t);

				break;
			}
			case EXTENDED_TYPE:
			{
				/* copy uint16_t header_size */
				be16_data = htobe16(it->extended.header_size);
				memcpy(buf + offset + sizeof(uint8_t), &be16_data, sizeof(uint16_t));

				/* copy uint8_t* header_value */
				memcpy(buf + offset + sizeof(uint8_t) + sizeof(uint16_t), it->extended.header_value, it->extended.header_size - (sizeof(uint8_t) + sizeof(uint16_t)));

				offset += it->extended.header_size;

				break;
			}
			default:
			{
				break;
			}
		}
		it = it->next;
	}

	return buflen;
}

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

	/* copy obex_packet_info_t if exists */
	switch (obex_packet->opcode)
	{
		case OBEX_CONNECT:
		{
			obex_packet->info = (struct obex_packet_info_t*) malloc(sizeof(struct obex_packet_info_t));
			assert(obex_packet->info != NULL);

			memcpy(obex_packet->info, buf + OFFSET_PACKET_INFO, sizeof(struct obex_packet_info_t));
			obex_packet->info->maximum_packet_length = be16toh(obex_packet->info->maximum_packet_length);

			break;
		}
		default:
		{
			obex_packet->info = NULL;

			break;
		}
	}

	offset = (obex_packet->info == NULL) ? (OBEX_MINIMUM_PACKET_SIZE) : (OBEX_MAXIMUM_PACKET_SIZE);
	while (offset < obex_packet->packet_length)
	{
		/* copy header_id */
		memcpy(&header_id, buf + offset, sizeof(uint8_t));

		switch (get_header_type(header_id))
		{
			case BASIC_TYPE:
			{
				/* copy header_value */
				memcpy(&hdr32_value, buf + offset + sizeof(uint8_t), sizeof(uint32_t));
				hdr32_value = be32toh(hdr32_value);

				obex_packet->headers = build_basic_header(obex_packet->headers, header_id, hdr32_value);
				offset += sizeof(uint8_t) + sizeof(uint32_t);

				break;
			}
			case EXTENDED_TYPE:
			{
				/* copy header_size */
				memcpy(&hdr16_size, buf + offset + sizeof(uint8_t), sizeof(uint16_t));
				hdr16_size = be16toh(hdr16_size);

				/* copy header_value */
				value_size = hdr16_size - sizeof(uint8_t) - sizeof(uint16_t);
				hdr8_value = (uint8_t*) malloc(value_size * sizeof(uint8_t));
				assert(hdr8_value != NULL);

				memcpy(hdr8_value, buf + offset + sizeof(uint8_t) + sizeof(uint16_t), sizeof(uint8_t));

				obex_packet->headers = build_extended_header(obex_packet->headers, header_id, hdr8_value, value_size);
				offset += sizeof(uint8_t) + sizeof(uint16_t) + value_size;

				break;
			}
			default:
			{
				break;
			}
		}
	}

	return buflen;
}

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

struct obex_packet_header_t* build_extended_header(struct obex_packet_header_t* headers, uint8_t header_id, uint8_t* header_value, uint16_t value_size)
{
	struct obex_packet_header_t* header;
	struct obex_packet_header_t* it;

	header = (struct obex_packet_header_t*) malloc(sizeof(struct obex_packet_header_t));
	assert(header != NULL);

	header->header_id = header_id;
	header->extended.header_size = sizeof(uint8_t) + sizeof(uint16_t) + value_size;
	header->extended.header_value = header_value;

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
