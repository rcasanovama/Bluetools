#include "l2cap.h"

/**
 * Internal function to clean the local l2cap socket.
 *
 * @param l2cap_socket handler of the l2cap connection
 */
static void internal_l2cap_cleanup_fd(struct l2cap_socket_t l2cap_socket)
{
	int r;

	r = shutdown(l2cap_socket.fd, SHUT_RDWR);
#ifndef NOVERBOSE
	if (r < 0)
	{
		perror("l2cap_shutdown_fd");
	}
#endif

	r = close(l2cap_socket.fd);
#ifndef NOVERBOSE
	if (r < 0)
	{
		perror("l2cap_close_fd");
	}
#endif
}

/**
 * Internal function to connect to remote device.
 *
 * @param l2cap_socket handler of the l2cap connection
 * @param addr address of the remote device
 * @param psm psm of the remote l2cap server
 * @return status code, -1 if error
 */
static int32_t internal_l2cap_connect(struct l2cap_socket_t l2cap_socket, const char* addr, uint8_t psm)
{
	struct sockaddr_l2 sockaddr;
	int r;

	sockaddr.l2_family = AF_BLUETOOTH;
	sockaddr.l2_psm = htobs(psm);
	str2ba(addr, &sockaddr.l2_bdaddr);

	r = connect(l2cap_socket.fd, (struct sockaddr*) &sockaddr, sizeof(sockaddr));
#ifndef NOVERBOSE
	if (r < 0)
	{
		perror("l2cap_connect");
	}
#endif

	return r;
}

/**
 * Internal function to accept incoming remote connections.
 *
 * @param l2cap_socket handler of the l2cap connection
 * @param connections number of incoming connection which will be accepted
 * @return file descriptor of the remote connection
 */
static int32_t internal_l2cap_accept(struct l2cap_socket_t l2cap_socket, uint8_t connections)
{
	struct sockaddr_l2 addr;
	socklen_t addrlen;
	int r;

	r = listen(l2cap_socket.fd, (connections == 0) ? (1) : (connections));
	if (r < 0)
	{
#ifndef NOVERBOSE
		perror("l2cap_listen");
#endif
		return r;
	}

	r = accept(l2cap_socket.fd, (struct sockaddr*) &addr, &addrlen);
#ifndef NOVERBOSE
	if (r < 0)
	{
		perror("l2cap_accept");
	}
#endif

	return r;

}

/**
 * Internal function to bind the local address to a psm.
 *
 * @param l2cap_socket handler of the l2cap connection
 * @param psm psm for listening incoming remote connections
 * @return status code, -1 if error
 */
static int8_t internal_l2cap_bind(struct l2cap_socket_t l2cap_socket, uint8_t psm)
{
	struct hci_dev_info dev_info;
	struct sockaddr_l2 addr;
	int r;

	if (l2cap_socket.fd < 0)
	{
		return - 1;
	}

	if (hci_devinfo(l2cap_socket.dev_id, &dev_info) < 0)
	{
		perror("hci_devinfo");
		return - 1;
	}

	memset(&addr, 0, sizeof(addr));

	addr.l2_family = AF_BLUETOOTH;
	addr.l2_psm = htobs(psm);
	addr.l2_bdaddr = dev_info.bdaddr;

	r = bind(l2cap_socket.fd, (struct sockaddr*) &addr, sizeof(addr));
#ifndef NOVERBOSE
	if (r < 0)
	{
		perror("l2cap_bind");
	}
#endif

	return r;
}

/**
 * Internal function to create a l2cap socket.
 *
 * @return handler of the l2cap connection
 */
static struct l2cap_socket_t internal_l2cap_socket()
{
	struct l2cap_socket_t l2cap_socket;

	memset(&l2cap_socket, 0, sizeof(l2cap_socket));

	l2cap_socket.fd = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
#ifndef NOVERBOSE
	if (l2cap_socket.fd < 0)
	{
		perror("l2cap_socket");
	}
#endif

	return l2cap_socket;
}

/**
 * Starts a l2cap connection with the specified remote device.
 *
 * @param dev_id identifier of the local bluetooth device
 * @param addr address of the remote device
 * @param psm psm of the remote l2cap server
 * @return handler of the l2cap connection
 */
struct l2cap_socket_t l2cap_client_socket(uint16_t dev_id, const char* addr, uint8_t psm)
{
	struct l2cap_socket_t l2cap_socket;
	int r;

	l2cap_socket = internal_l2cap_socket();
	if (l2cap_socket.fd < 0)
	{
		return l2cap_socket;
	}

	l2cap_socket.dev_id = dev_id;

	r = internal_l2cap_connect(l2cap_socket, addr, psm);
	if (r < 0)
	{
		internal_l2cap_cleanup_fd(l2cap_socket);
		l2cap_socket.fd = - 1;

		return l2cap_socket;
	}

	l2cap_socket.rmt_fd = l2cap_socket.fd;

	return l2cap_socket;
}

/**
 * Starts a l2cap server listening on the specified psm.
 *
 * @param dev_id identifier of the local bluetooth device
 * @param psm psm for listening incoming remote connections
 * @param connections number of incoming connection which will be accepted
 * @return handler of the l2cap connection
 */
struct l2cap_socket_t l2cap_server_socket(uint16_t dev_id, uint8_t psm, uint8_t connections)
{
	struct l2cap_socket_t l2cap_socket;
	int r;

	l2cap_socket = internal_l2cap_socket();
	if (l2cap_socket.fd < 0)
	{
		return l2cap_socket;
	}

	l2cap_socket.dev_id = dev_id;

	r = internal_l2cap_bind(l2cap_socket, psm);
	if (r < 0)
	{
		internal_l2cap_cleanup_fd(l2cap_socket);
		l2cap_socket.fd = - 1;

		return l2cap_socket;
	}

	l2cap_socket.rmt_fd = internal_l2cap_accept(l2cap_socket, connections);
	if (l2cap_socket.rmt_fd < 0)
	{
		internal_l2cap_cleanup_fd(l2cap_socket);
		l2cap_socket.fd = - 1;

		return l2cap_socket;
	}

	return l2cap_socket;
}

/**
 * Sends a message to the remote device using the l2cap connection.
 *
 * @param l2cap_socket handler of the l2cap connection
 * @param buf buf where the message is stored
 * @param buflen size of the buf
 * @return number of bytes sent, will be equals to the buflen
 */
ssize_t l2cap_send(struct l2cap_socket_t l2cap_socket, const void* buf, size_t buflen)
{
	ssize_t send_len;

	send_len = send(l2cap_socket.rmt_fd, buf, buflen, 0);
#ifndef NOVERBOSE
	if (send_len < 0)
	{
		perror("l2cap_send");
	}
#endif

	return send_len;
}

/**
 * Receives a message from the remote device using the l2cap connection.
 *
 * @param l2cap_socket handler of the l2cap connection
 * @param buf buf where the message will be stored
 * @param buflen size of the buf
 * @return number of bytes received
 */
ssize_t l2cap_recv(struct l2cap_socket_t l2cap_socket, void* buf, size_t buflen)
{
	ssize_t recv_len;

	recv_len = recv(l2cap_socket.rmt_fd, buf, buflen, 0);
#ifndef NOVERBOSE
	if (recv_len < 0)
	{
		perror("l2cap_recv");
	}
#endif

	return recv_len;
}

/**
 * Cleans the l2cap connection.
 *
 * @param l2cap_socket handler of the l2cap connection
 */
void l2cap_cleanup(struct l2cap_socket_t l2cap_socket)
{
	int r;

	if (l2cap_socket.rmt_fd != l2cap_socket.fd)
	{
		r = shutdown(l2cap_socket.rmt_fd, SHUT_RDWR);
#ifndef NOVERBOSE
		if (r < 0)
		{
			perror("l2cap shutdown rfd");
		}
#endif

		r = close(l2cap_socket.rmt_fd);
#ifndef NOVERBOSE
		if (r < 0)
		{
			perror("l2cap close rfd");
		}
#endif
	}

	r = shutdown(l2cap_socket.fd, SHUT_RDWR);
#ifndef NOVERBOSE
	if (r < 0)
	{
		perror("l2cap shutdown fd");
	}
#endif

	r = close(l2cap_socket.fd);
#ifndef NOVERBOSE
	if (r < 0)
	{
		perror("l2cap close fd");
	}
#endif
}