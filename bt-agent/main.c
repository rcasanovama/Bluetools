#include "../src/bt-agent.h"

int main()
{
	char buf[1024];
	int client;

	client = agent_init_rfcomm_client(1, "00:00:00:00:00:00");
	if (client >= 0)
	{
		agent_write_data(client, "message\n");
		agent_read_data(client, buf, 1024);
		agent_cleanup(client);
	}

	return 0;
}
