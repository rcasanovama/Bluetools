#include "l2cap.h"

static void internal_l2cap_cleanup_fd(struct l2cap_socket_t __l2cap_socket_t)
{
	int r;

	r = shutdown(__l2cap_socket_t.fd, SHUT_RDWR);
#ifndef NOVERBOSE
	if (r < 0)
	{
		perror("l2cap_shutdown_fd");
	}
#endif

	r = close(__l2cap_socket_t.fd);
#ifndef NOVERBOSE
	if (r < 0)
	{
		perror("l2cap_close_fd");
	}
#endif
}

static int32_t internal_l2cap_connect(struct l2cap_socket_t __l2cap_socket_t, const char* __addr, uint8_t __psm)
{
	struct sockaddr_l2 addr;
	int r;

	addr.l2_family = AF_BLUETOOTH;
	addr.l2_psm = htobs(__psm);
	str2ba(__addr, &addr.l2_bdaddr);

	r = connect(__l2cap_socket_t.fd, (struct sockaddr*) &addr, sizeof(addr));
#ifndef NOVERBOSE
	if (r < 0)
	{
		perror("l2cap_connect");
	}
#endif

	return r;
}

static int32_t internal_l2cap_accept(struct l2cap_socket_t __l2cap_socket_t, uint8_t __connections)
{
	struct sockaddr_l2 addr;
	socklen_t addrlen;
	int r;

	r = listen(__l2cap_socket_t.fd, (__connections == 0) ? (1) : (__connections));
	if (r < 0)
	{
#ifndef NOVERBOSE
		perror("l2cap_listen");
#endif
		return r;
	}

	r = accept(__l2cap_socket_t.fd, (struct sockaddr*) &addr, &addrlen);
#ifndef NOVERBOSE
	if (r < 0)
	{
		perror("l2cap_accept");
	}
#endif

	return r;

}

static int8_t internal_l2cap_bind(struct l2cap_socket_t __l2cap_socket_t, uint8_t __psm)
{
	struct hci_dev_info dev_info;
	struct sockaddr_l2 addr;
	int r;

	if (__l2cap_socket_t.fd < 0)
	{
		return - 1;
	}

	if (hci_devinfo(__l2cap_socket_t.dev_id, &dev_info) < 0)
	{
		perror("hci_devinfo");
		return - 1;
	}

	memset(&addr, 0, sizeof(addr));

	addr.l2_family = AF_BLUETOOTH;
	addr.l2_psm = htobs(__psm);
	addr.l2_bdaddr = dev_info.bdaddr;

	r = bind(__l2cap_socket_t.fd, (struct sockaddr*) &addr, sizeof(addr));
#ifndef NOVERBOSE
	if (r < 0)
	{
		perror("l2cap_bind");
	}
#endif

	return r;
}

static struct l2cap_socket_t internal_l2cap_socket()
{
	struct l2cap_socket_t __l2cap_socket_t;

	memset(&__l2cap_socket_t, 0, sizeof(__l2cap_socket_t));

	__l2cap_socket_t.fd = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
#ifndef NOVERBOSE
	if (__l2cap_socket_t.fd < 0)
	{
		perror("l2cap_socket");
	}
#endif

	return __l2cap_socket_t;
}

struct l2cap_socket_t l2cap_client_socket(uint16_t dev_id, const char* __addr, uint8_t __psm)
{
	struct l2cap_socket_t __l2cap_socket_t;
	int r;

	__l2cap_socket_t = internal_l2cap_socket();
	if (__l2cap_socket_t.fd < 0)
	{
		return __l2cap_socket_t;
	}

	__l2cap_socket_t.dev_id = dev_id;

	r = internal_l2cap_connect(__l2cap_socket_t, __addr, __psm);
	if (r < 0)
	{
		internal_l2cap_cleanup_fd(__l2cap_socket_t);
		__l2cap_socket_t.fd = - 1;

		return __l2cap_socket_t;
	}

	__l2cap_socket_t.rmt_fd = __l2cap_socket_t.fd;

	return __l2cap_socket_t;
}

struct l2cap_socket_t l2cap_server_socket(uint16_t dev_id, uint8_t __psm, uint8_t __connections)
{
	struct l2cap_socket_t __l2cap_socket_t;
	int r;

	__l2cap_socket_t = internal_l2cap_socket();
	if (__l2cap_socket_t.fd < 0)
	{
		return __l2cap_socket_t;
	}

	__l2cap_socket_t.dev_id = dev_id;

	r = internal_l2cap_bind(__l2cap_socket_t, __psm);
	if (r < 0)
	{
		internal_l2cap_cleanup_fd(__l2cap_socket_t);
		__l2cap_socket_t.fd = - 1;

		return __l2cap_socket_t;
	}

	__l2cap_socket_t.rmt_fd = internal_l2cap_accept(__l2cap_socket_t, __connections);
	if (__l2cap_socket_t.rmt_fd < 0)
	{
		internal_l2cap_cleanup_fd(__l2cap_socket_t);
		__l2cap_socket_t.fd = - 1;

		return __l2cap_socket_t;
	}

	return __l2cap_socket_t;
}

ssize_t l2cap_send(struct l2cap_socket_t __l2cap_socket_t, const void* buf, size_t buflen)
{
	ssize_t send_len;

	send_len = send(__l2cap_socket_t.rmt_fd, buf, buflen, 0);
#ifndef NOVERBOSE
	if (send_len < 0)
	{
		perror("l2cap_send");
	}
#endif

	return send_len;
}

ssize_t l2cap_recv(struct l2cap_socket_t __l2cap_socket_t, void* buf, size_t buflen)
{
	ssize_t recv_len;

	recv_len = recv(__l2cap_socket_t.rmt_fd, buf, buflen, 0);
#ifndef NOVERBOSE
	if (recv_len < 0)
	{
		perror("l2cap_recv");
	}
#endif

	return recv_len;
}

void l2cap_cleanup(struct l2cap_socket_t __l2cap_socket_t)
{
	int r;

	if (__l2cap_socket_t.rmt_fd != __l2cap_socket_t.fd)
	{
		r = shutdown(__l2cap_socket_t.rmt_fd, SHUT_RDWR);
#ifndef NOVERBOSE
		if (r < 0)
		{
			perror("l2cap shutdown rfd");
		}
#endif

		r = close(__l2cap_socket_t.rmt_fd);
#ifndef NOVERBOSE
		if (r < 0)
		{
			perror("l2cap close rfd");
		}
#endif
	}

	r = shutdown(__l2cap_socket_t.fd, SHUT_RDWR);
#ifndef NOVERBOSE
	if (r < 0)
	{
		perror("l2cap shutdown fd");
	}
#endif

	r = close(__l2cap_socket_t.fd);
#ifndef NOVERBOSE
	if (r < 0)
	{
		perror("l2cap close fd");
	}
#endif
}