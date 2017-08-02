#ifndef _UDP_H
#define _UDP_H

#if defined(__cplusplus)
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <string.h>

#ifdef _WIN32

#include <winsock2.h>
#include <windows.h>

#endif // _WIN32

#ifdef __linux__

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netdb.h>

#endif // __linux__

#ifdef __APPLE__

#endif // __APPLE__


#define UDP_V4 AF_INET
#define UDP_V6 AF_INET6

struct udp_socket_t
{
	int32_t fd;

	uint16_t domain;
	uint16_t type;
	uint16_t protocol;

	uint16_t port;
};


extern int8_t init_networking();

extern int8_t cleanup_networking();


extern struct udp_socket_t* udp_socket(uint16_t __domain, uint16_t __protocol);

extern int8_t udp_bind(struct udp_socket_t* __udp_socket_t, uint16_t __port);


bool send_udp_message(int sock_fd, struct in_addr dst_address, unsigned short port, const char* buffer, unsigned int buffer_length);


bool receive_udp_message(int sock_fd, unsigned int timeout, char* buffer, unsigned int buffer_length, int* received_length);

bool close_socket(int sock_fd);

#if defined(__cplusplus)
}
#endif

#endif // _UDP_H
