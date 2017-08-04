#ifndef _UDP_H
#define _UDP_H

#if defined(__cplusplus)
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

struct udp_socket_t
{
	int32_t fd;

	uint16_t domain;
	uint16_t type;
	uint16_t protocol;

//	uint16_t port;
};

extern struct udp_socket_t udp_client_socket(uint16_t __domain);

extern struct udp_socket_t udp_server_socket(uint16_t __domain, uint16_t __port);

extern ssize_t udp_sendto(struct udp_socket_t __udp_socket_t, const char* __name, uint16_t __port, const void* buf, size_t buflen);

extern ssize_t udp_recvfrom(struct udp_socket_t __udp_socket_t, void* buf, size_t buflen);

extern int8_t udp_cleanup(struct udp_socket_t __udp_socket_t);

#if defined(__cplusplus)
}
#endif

#endif // _UDP_H
