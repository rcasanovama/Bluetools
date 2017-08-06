#include "../src/bt-agent.h"

int main()
{
	char buf[1024];
	int client;

	client = agent_init_l2cap_server(25);
	if (client >= 0)
	{
		printf("conectado\n");
//		agent_write_data(client, "AT\r\n");
		agent_read_data(client, buf, 1024);
		printf("Data: %s\n", buf);
		agent_cleanup(client);
	}

	return 0;
}
