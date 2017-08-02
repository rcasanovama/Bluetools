#include "udp.h"

int8_t init_networking()
{
#ifdef _WIN32
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return - 1;
	}
#endif

	return 0;
}

int8_t cleanup_networking()
{
#ifdef _WIN32
	WSACleanup();
#endif

	return 0;
}

static int8_t internal_udp_bind(struct udp_socket_t __udp_socket_t, uint16_t __port)
{
	struct sockaddr_in addr;
	struct sockaddr_in6 addr6;
	int r;

	if (__udp_socket_t.fd < 0)
	{
		return - 1;
	}

	switch (__udp_socket_t.domain)
	{
		case AF_INET:
		{
			memset(&addr, 0, sizeof(addr));

			addr.sin_family = AF_INET;
			addr.sin_port = htons(__port);
			addr.sin_addr.s_addr = htonl(INADDR_ANY);

			r = bind(__udp_socket_t.fd, (struct sockaddr*) &addr, sizeof(addr));
#ifndef NOVERBOSE
			if (r < 0)
			{
				perror("bind4");
			}
#endif

			break;
		}
		case AF_INET6:
		{
			memset(&addr6, 0, sizeof(addr6));

			addr6.sin6_family = AF_INET6;
			addr6.sin6_port = htons(__port);
			addr6.sin6_addr = in6addr_any;

			r = bind(__udp_socket_t.fd, (struct sockaddr*) &addr6, sizeof(addr6));
#ifndef NOVERBOSE
			if (r < 0)
			{
				perror("bind6");
			}
#endif

			break;
		}
		default:
		{
			// TODO: handle more domains
			r = - 1;

			break;
		}
	}

	return r;
}

static struct udp_socket_t internal_udp_socket(uint16_t __domain)
{
	struct udp_socket_t __udp_socket_t;

	memset(&__udp_socket_t, 0, sizeof(__udp_socket_t));

	__udp_socket_t.fd = socket(__domain, SOCK_DGRAM, IPPROTO_UDP);
#ifndef NOVERBOSE
	if (__udp_socket_t.fd < 0)
	{
		perror("udp_socket");
	}
#endif

	__udp_socket_t.domain = __domain;
	__udp_socket_t.type = SOCK_DGRAM;
	__udp_socket_t.protocol = IPPROTO_UDP;

	return __udp_socket_t;
}

struct udp_socket_t udp_client_socket(uint16_t __domain)
{
	struct udp_socket_t __udp_socket_t;

	// TODO: show error messages if internal_udp_socket fails
	__udp_socket_t = internal_udp_socket(__domain);
#ifndef NOVERBOSE
	if (__udp_socket_t.fd < 0)
	{
//		fprintf(stderr, "client");
	}
#endif

	return __udp_socket_t;
}

struct udp_socket_t udp_server_socket(uint16_t __domain, uint16_t __port)
{
	struct udp_socket_t __udp_socket_t;
	int r;

	// TODO: show error messages if internal_udp_socket fails
	__udp_socket_t = internal_udp_socket(__domain);
#ifndef NOVERBOSE
	if (__udp_socket_t.fd < 0)
	{
//		fprintf(stderr, "server");
	}
#endif

	__udp_socket_t.port = __port;

	// TODO: show error messages if internal_udp_bind fails
	r = internal_udp_bind(__udp_socket_t, __port);
#ifndef NOVERBOSE
	if (r < 0)
	{
//		fprintf(stderr, "server_bind");
	}
#endif

	return __udp_socket_t;
}


bool send_udp_message(int sock_fd, struct in_addr dst_address, unsigned short port, const char* buffer, unsigned int buffer_length)
{
	struct sockaddr_in dst_sockaddr;

	memset(&dst_sockaddr, 0, sizeof(dst_sockaddr));

	dst_sockaddr.sin_family = AF_INET;
	dst_sockaddr.sin_port = htons(port);
	dst_sockaddr.sin_addr = dst_address;

	return sendto(sock_fd, buffer, buffer_length, 0, (struct sockaddr*) &dst_sockaddr, sizeof(dst_sockaddr)) >= 0;
}

bool receive_udp_message(int sock_fd, unsigned int timeout, char* buffer, unsigned int buffer_length, int* received_length)
{
	struct sockaddr_in src_sockaddr;

#ifdef _WIN32
	int src_sockaddr_length = sizeof(src_sockaddr);
#else
	unsigned int src_sockaddr_length = sizeof(src_sockaddr);
#endif

	fd_set set;
	struct timeval time_value, * ptr_time_value;

	FD_ZERO(&set);
	FD_SET(sock_fd, &set);
	if (timeout > 0)
	{
		time_value.tv_sec = timeout / 1000;
		time_value.tv_usec = 1000 * (timeout % 1000);
		ptr_time_value = &time_value;
	}
	else
	{
		ptr_time_value = NULL;
	}

	if (select(sock_fd + 1, &set, NULL, NULL, ptr_time_value) <= 0)
	{
		*received_length = 0;
		return false;
	}

	*received_length = recvfrom(sock_fd, buffer, buffer_length, 0, (struct sockaddr*) &src_sockaddr, &src_sockaddr_length);

	return *received_length >= 0;
}

bool close_socket(int sock_fd)
{
#ifdef _WIN32
	return closesocket(sock_fd) == 0;
#else
	return shutdown(sock_fd, SHUT_RDWR) == 0;
#endif
}