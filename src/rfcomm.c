#include "rfcomm.h"

static void internal_rfcomm_cleanup_fd(struct rfcomm_socket_t __rfcomm_socket_t)
{
	int r;

	r = shutdown(__rfcomm_socket_t.fd, SHUT_RDWR);
#ifndef NOVERBOSE
	if (r < 0)
	{
		perror("rfcomm_shutdown_fd");
	}
#endif

	r = close(__rfcomm_socket_t.fd);
#ifndef NOVERBOSE
	if (r < 0)
	{
		perror("rfcomm_close_fd");
	}
#endif
}

static int32_t internal_rfcomm_connect(struct rfcomm_socket_t __rfcomm_socket_t, const char* __addr, uint8_t __channel)
{
	struct sockaddr_rc addr;
	int r;

	addr.rc_family = AF_BLUETOOTH;
	addr.rc_channel = __channel;
	str2ba(__addr, &addr.rc_bdaddr);

	r = connect(__rfcomm_socket_t.fd, (struct sockaddr*) &addr, sizeof(addr));
#ifndef NOVERBOSE
	if (r < 0)
	{
		perror("rfcomm_connect");
	}
#endif

	return r;
}

static int32_t internal_rfcomm_accept(struct rfcomm_socket_t __rfcomm_socket_t, uint8_t __connections)
{
	struct sockaddr_rc addr;
	socklen_t addrlen;
	int r;

	r = listen(__rfcomm_socket_t.fd, (__connections == 0) ? (1) : (__connections));
	if (r < 0)
	{
#ifndef NOVERBOSE
		perror("rfcomm_listen");
#endif
		return r;
	}

	r = accept(__rfcomm_socket_t.fd, (struct sockaddr*) &addr, &addrlen);
#ifndef NOVERBOSE
	if (r < 0)
	{
		perror("rfcomm_accept");
	}
#endif

	return r;

}

static int8_t internal_rfcomm_bind(struct rfcomm_socket_t __rfcomm_socket_t, uint8_t __channel)
{
	struct hci_dev_info dev_info;
	struct sockaddr_rc addr;
	int r;

	if (__rfcomm_socket_t.fd < 0)
	{
		return - 1;
	}

	if (hci_devinfo(__rfcomm_socket_t.dev_id, &dev_info) < 0)
	{
		perror("hci_devinfo");
		return - 1;
	}

	memset(&addr, 0, sizeof(addr));

	addr.rc_family = AF_BLUETOOTH;
	addr.rc_channel = __channel;
	addr.rc_bdaddr = dev_info.bdaddr;

	r = bind(__rfcomm_socket_t.fd, (struct sockaddr*) &addr, sizeof(addr));
#ifndef NOVERBOSE
	if (r < 0)
	{
		perror("rfcomm_bind");
	}
#endif

	return r;
}

static struct rfcomm_socket_t internal_rfcomm_socket()
{
	struct rfcomm_socket_t __rfcomm_socket_t;

	memset(&__rfcomm_socket_t, 0, sizeof(__rfcomm_socket_t));

	__rfcomm_socket_t.fd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
#ifndef NOVERBOSE
	if (__rfcomm_socket_t.fd < 0)
	{
		perror("rfcomm_socket");
	}
#endif

	return __rfcomm_socket_t;
}

struct rfcomm_socket_t rfcomm_client_socket(uint16_t dev_id, const char* __addr, uint8_t __channel)
{
	struct rfcomm_socket_t __rfcomm_socket_t;
	int r;

	__rfcomm_socket_t = internal_rfcomm_socket();
	if (__rfcomm_socket_t.fd < 0)
	{
		return __rfcomm_socket_t;
	}

	__rfcomm_socket_t.dev_id = dev_id;

	r = internal_rfcomm_connect(__rfcomm_socket_t, __addr, __channel);
	if (r < 0)
	{
		internal_rfcomm_cleanup_fd(__rfcomm_socket_t);
		__rfcomm_socket_t.fd = - 1;

		return __rfcomm_socket_t;
	}

	__rfcomm_socket_t.rmt_fd = __rfcomm_socket_t.fd;

	return __rfcomm_socket_t;
}

struct rfcomm_socket_t rfcomm_server_socket(uint16_t dev_id, uint8_t __channel, uint8_t __connections)
{
	struct rfcomm_socket_t __rfcomm_socket_t;
	int r;

	__rfcomm_socket_t = internal_rfcomm_socket();
	if (__rfcomm_socket_t.fd < 0)
	{
		return __rfcomm_socket_t;
	}

	__rfcomm_socket_t.dev_id = dev_id;

	r = internal_rfcomm_bind(__rfcomm_socket_t, __channel);
	if (r < 0)
	{
		internal_rfcomm_cleanup_fd(__rfcomm_socket_t);
		__rfcomm_socket_t.fd = - 1;

		return __rfcomm_socket_t;
	}

	__rfcomm_socket_t.rmt_fd = internal_rfcomm_accept(__rfcomm_socket_t, __connections);
	if (__rfcomm_socket_t.rmt_fd < 0)
	{
		internal_rfcomm_cleanup_fd(__rfcomm_socket_t);
		__rfcomm_socket_t.fd = - 1;

		return __rfcomm_socket_t;
	}

	return __rfcomm_socket_t;
}

ssize_t rfcomm_send(struct rfcomm_socket_t __rfcomm_socket_t, const void* buf, size_t buflen)
{
	ssize_t send_len;

	send_len = send(__rfcomm_socket_t.rmt_fd, buf, buflen, 0);
#ifndef NOVERBOSE
	if (send_len < 0)
	{
		perror("rfcomm_send");
	}
#endif

	return send_len;
}

ssize_t rfcomm_recv(struct rfcomm_socket_t __rfcomm_socket_t, void* buf, size_t buflen)
{
	ssize_t recv_len;

	recv_len = recv(__rfcomm_socket_t.rmt_fd, buf, buflen, 0);
#ifndef NOVERBOSE
	if (recv_len < 0)
	{
		perror("rfcomm_recv");
	}
#endif

	return recv_len;
}

void rfcomm_cleanup(struct rfcomm_socket_t __rfcomm_socket_t)
{
	int r;

	if (__rfcomm_socket_t.rmt_fd != __rfcomm_socket_t.fd)
	{
		r = shutdown(__rfcomm_socket_t.rmt_fd, SHUT_RDWR);
#ifndef NOVERBOSE
		if (r < 0)
		{
			perror("rfcomm shutdown rfd");
		}
#endif

		r = close(__rfcomm_socket_t.rmt_fd);
#ifndef NOVERBOSE
		if (r < 0)
		{
			perror("rfcomm close rfd");
		}
#endif
	}

	r = shutdown(__rfcomm_socket_t.fd, SHUT_RDWR);
#ifndef NOVERBOSE
	if (r < 0)
	{
		perror("rfcomm shutdown fd");
	}
#endif

	r = close(__rfcomm_socket_t.fd);
#ifndef NOVERBOSE
	if (r < 0)
	{
		perror("rfcomm close fd");
	}
#endif
}