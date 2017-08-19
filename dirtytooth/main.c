#include "src/obex.h"
#include "src/pbap.h"

int main()
{
	struct obex_t obex;
	struct obex_packet_header_t* headers;

	obex = obex_init(0, "00:00:00:00:00:00", 13);
	if (obex.obex_status != OBEX_INIT_OK)
	{
		return 1;
	}

	headers = build_extended_header(NULL, OBEX_TARGET, UUID_PBAP, sizeof(UUID_PBAP));
	obex_connect(&obex, headers);

	/* pull phonebook */
//	uint8_t pb[30] = {0x00, 0x74, 0x00, 0x65, 0x00, 0x6C, 0x00, 0x65, 0x00, 0x63, 0x00, 0x6F, 0x00, 0x6D, 0x00, 0x2F, 0x00, 0x70, 0x00, 0x62, 0x00, 0x2E, 0x00, 0x76, 0x00, 0x63, 0x00, 0x66, 0x00, 0x00};
//	uint8_t type[15] = {0x78, 0x2d, 0x62, 0x74, 0x2f, 0x70, 0x68, 0x6f, 0x6e, 0x65, 0x62, 0x6f, 0x6f, 0x6b, 0x00};
//	headers = build_basic_header(NULL, OBEX_CONNECTION_ID, obex.connection_id);
//	headers = build_extended_header(headers, OBEX_NAME, pb, sizeof(pb));
//	headers = build_extended_header(headers, OBEX_TYPE, type, sizeof(type));
//	obex_get_final(&obex, headers);
//	obex_get_final(&obex, NULL);
//	obex_get_final(&obex, NULL);

	/* pull call history */
//	headers = build_basic_header(NULL, OBEX_CONNECTION_ID, obex.connection_id);
//	headers = build_basic_header(headers, OBEX_CONNECTION_ID, obex.connection_id);
//	headers = build_basic_header(headers, OBEX_CONNECTION_ID, obex.connection_id);
//	obex_get(&obex, headers);

	headers = build_basic_header(NULL, OBEX_CONNECTION_ID, obex.connection_id);
	obex_disconnect(&obex, headers);

	obex_cleanup(obex);

	return 0;
}
