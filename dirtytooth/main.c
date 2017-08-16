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

	// obex_disconnect(obex, NULL);

	obex_cleanup(obex);

	return 0;
}
