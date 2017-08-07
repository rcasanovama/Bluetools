#ifndef __BLUETOOTH_L2CAP_H
#define __BLUETOOTH_L2CAP_H

#if defined(__cplusplus)
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>

struct l2cap_socket_t
{
	uint16_t dev_id;

	int32_t fd;
	int32_t rmt_fd; // remote fd for accept or connect

//	uint16_t domain;
//	uint16_t type;
//	uint16_t protocol;

//	uint8_t psm;
};

extern struct l2cap_socket_t l2cap_client_socket(uint16_t dev_id, const char* __addr, uint8_t __psm);

extern struct l2cap_socket_t l2cap_server_socket(uint16_t dev_id, uint8_t __psm, uint8_t __connections);

extern ssize_t l2cap_send(struct l2cap_socket_t __l2cap_socket_t, const void* buf, size_t buflen);

extern ssize_t l2cap_recv(struct l2cap_socket_t __l2cap_socket_t, void* buf, size_t buflen);

extern int8_t l2cap_cleanup(struct l2cap_socket_t __l2cap_socket_t);

#if defined(__cplusplus)
}
#endif

#endif // __BLUETOOTH_L2CAP_H
