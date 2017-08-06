/**
 * Author:  Raul Casanova Marques
 * Date:    16/07/17
 */
#include "bt-agent.h"

int agent_init_rfcomm_server(uint8_t channel)
{
	struct sockaddr_rc local_addr, remote_addr;
	socklen_t addr_len;
	int fd, client, result;

	fd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	if (fd < 0)
	{
		perror("rfcomm_socket");
		return fd; // -1
	}

	local_addr.rc_family = AF_BLUETOOTH;
	local_addr.rc_bdaddr = *BDADDR_ANY;
	local_addr.rc_channel = channel;

	if ((result = bind(fd, (struct sockaddr*) &local_addr, sizeof(local_addr))) < 0)
	{
		perror("rfcomm_bind");
		return result; // -1
	}

	if ((result = listen(fd, 1)) < 0)
	{
		perror("rfcomm_listen");
		return result; // -1
	}

	addr_len = sizeof(remote_addr);
	if ((client = accept(fd, (struct sockaddr*) &remote_addr, &addr_len)) < 0)
	{
		perror("rfcomm_accept");
		return client; // -1
	}

	return client;
}

int agent_init_rfcomm_client(uint8_t channel, char* address)
{
	struct sockaddr_rc addr;
	int fd, result;

	fd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	if (fd < 0)
	{
		perror("rfcomm_socket");
		return fd; // -1
	}

	addr.rc_family = AF_BLUETOOTH;
	addr.rc_channel = channel;
	str2ba(address, &addr.rc_bdaddr);

	if ((result = connect(fd, (struct sockaddr*) &addr, sizeof(addr))) < 0)
	{
		perror("rfcomm_connect");
		return result;
	}

	return fd;
}

int agent_init_l2cap_server(unsigned short l2_psm)
{
	struct sockaddr_l2 addr;
	struct sockaddr_l2 r_addr;
	socklen_t r_addrlen;

	int fd, client, r;

	memset(&addr, 0, sizeof(addr));

	fd = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
	if (fd < 0)
	{
#ifndef NOVERBOSE
		perror("l2cap_socket");
#endif
		return - 1;
	}

	addr.l2_family = AF_BLUETOOTH;
	addr.l2_psm = htobs(l2_psm);
	addr.l2_bdaddr = htobs(*BDADDR_ANY);

	r = bind(fd, (struct sockaddr*) &addr, sizeof(addr));
	if (r < 0)
	{
#ifndef NOVERBOSE
		perror("l2cap_bind");
#endif
		return - 1;
	}
	printf("bind ok\n");

	r = listen(fd, 1);
	if (r < 0)
	{
#ifndef NOVERBOSE
		perror("l2cap_listen");
#endif
		return - 1;
	}
	printf("listen ok\n");

	client = accept(fd, (struct sockaddr*) &r_addr, &r_addrlen);
	if (client < 0)
	{
#ifndef NOVERBOSE
		perror("l2cap_accept");
#endif
		return client; // -1
	}
	printf("accept ok\n");

	return client;
}

int agent_init_l2cap_client(unsigned short l2_psm, char* address)
{
	struct sockaddr_l2 addr;
	int fd, result;

	fd = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
	if (fd < 0)
	{
		perror("l2cap_socket");
		return fd; // -1
	}

	addr.l2_family = AF_BLUETOOTH;
	addr.l2_psm = htobs(l2_psm);
//	addr.l2_psm = htobs(0x1001);
	str2ba(address, &addr.l2_bdaddr);

	if ((result = connect(fd, (struct sockaddr*) &addr, sizeof(addr))) < 0)
	{
		perror("l2cap_connect");
		return result;
	}

	return fd;
}

ssize_t agent_read_data(int client, char* buf, size_t buf_size)
{
	ssize_t result;
	assert(buf != NULL);

	memset(buf, 0, buf_size); // buf cleanup
	if ((result = read(client, buf, buf_size)) < 0)
	{
		perror("read");
	}
	return result;
}

ssize_t agent_write_data(int client, char* buf)
{
	ssize_t result;
	assert(buf != NULL);

	if ((result = write(client, buf, strlen(buf))) < 0)
	{
		perror("write");
	}
	return result;
}

void agent_cleanup(int fd)
{
	if (fd >= 0) // no errors
	{
		close(fd);
	}
}