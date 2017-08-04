#include "udp.h"

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

ssize_t udp_sendto(struct udp_socket_t __udp_socket_t, const char* __name, uint16_t __port, const void* buf, size_t buflen)
{
	socklen_t addrlen;
	struct sockaddr* addr;

	struct sockaddr_in dest_addr;
	struct sockaddr_in6 dest_addr6;

	struct addrinfo req, * pai;
	ssize_t send_len;
	int r;

	memset(&req, 0, sizeof(req));

	req.ai_family = AF_UNSPEC; // IPv4 | IPv6
	req.ai_socktype = __udp_socket_t.type;
	req.ai_protocol = IPPROTO_UDP;
	req.ai_flags |= AI_CANONNAME;

	r = getaddrinfo(__name, NULL, &req, &pai);
#ifndef NOVERBOSE
	if (r != 0)
	{
		perror("getaddrinfo");
	}
#endif

	send_len = - 1;
	while (pai)
	{
		switch (pai->ai_family)
		{
			case AF_INET:
			{
				memset(&dest_addr, 0, sizeof(struct sockaddr_in));

				dest_addr.sin_family = __udp_socket_t.domain;
				dest_addr.sin_port = htons(__port);
				dest_addr.sin_addr = ((struct sockaddr_in*) pai->ai_addr)->sin_addr;

				addr = (struct sockaddr*) &dest_addr;
				addrlen = sizeof(struct sockaddr_in);

				break;
			}
			case AF_INET6:
			{
				memset(&dest_addr6, 0, sizeof(struct sockaddr_in6));

				dest_addr6.sin6_family = __udp_socket_t.domain;
				dest_addr6.sin6_port = htons(__port);
				dest_addr6.sin6_addr = ((struct sockaddr_in6*) pai->ai_addr)->sin6_addr;

				addr = (struct sockaddr*) &dest_addr6;
				addrlen = sizeof(struct sockaddr_in6);

				break;
			}
			default:
			{
				addr = NULL;
				addrlen = 0;

				break;
			}
		}

		if (addr != NULL)
		{
			send_len = sendto(__udp_socket_t.fd, buf, buflen, 0, addr, addrlen);
			if (send_len < 0)
			{
#ifndef NOVERBOSE
				perror("udp_sendto");
#endif
				freeaddrinfo(pai);

				return send_len;
			}
		}

		pai = pai->ai_next;
	}

	freeaddrinfo(pai);

	return send_len;
}

ssize_t udp_recvfrom(struct udp_socket_t __udp_socket_t, void* buf, size_t buflen)
{
	socklen_t addrlen;
	struct sockaddr* addr;

	struct sockaddr_in dest_addr;
	struct sockaddr_in6 dest_addr6;

	ssize_t recv_len;

	recv_len = - 1;
	switch (__udp_socket_t.domain)
	{
		case AF_INET:
		{
			memset(&dest_addr, 0, sizeof(struct sockaddr_in));

			addr = (struct sockaddr*) &dest_addr;

			break;
		}
		case AF_INET6:
		{
			memset(&dest_addr6, 0, sizeof(struct sockaddr_in6));

			addr = (struct sockaddr*) &dest_addr6;

			break;
		}
		default:
		{
			addr = NULL;

			break;
		}
	}

	if (addr != NULL)
	{
		recv_len = recvfrom(__udp_socket_t.fd, buf, buflen, 0, addr, &addrlen);
#ifndef NOVERBOSE
		if (recv_len < 0)
		{
			perror("udp_recvfrom");
		}
#endif
	}

	return recv_len;
}

int8_t udp_cleanup(struct udp_socket_t __udp_socket_t)
{
	int r;

	r = shutdown(__udp_socket_t.fd, SHUT_RDWR);
#ifndef NOVERBOSE
	if (r < 0)
	{
		perror("udp_cleanup");
	}
#endif

	return r;
}