#ifndef __BLUETOOTH_RFCOMM_H
#define __BLUETOOTH_RFCOMM_H

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
#include <bluetooth/rfcomm.h>

struct rfcomm_socket_t
{
	uint16_t dev_id;

	int32_t fd;
	int32_t rmt_fd; // remote fd for accept or connect

//	uint16_t domain;
//	uint16_t type;
//	uint16_t protocol;

//	uint8_t channel;
};

extern struct rfcomm_socket_t rfcomm_client_socket(uint16_t dev_id, const char* __addr, uint8_t __channel);

extern struct rfcomm_socket_t rfcomm_server_socket(uint16_t dev_id, uint8_t __channel, uint8_t __connections);

extern ssize_t rfcomm_send(struct rfcomm_socket_t __rfcomm_socket_t, const void* buf, size_t buflen);

extern ssize_t rfcomm_recv(struct rfcomm_socket_t __rfcomm_socket_t, void* buf, size_t buflen);

extern int8_t rfcomm_cleanup(struct rfcomm_socket_t __rfcomm_socket_t);

#if defined(__cplusplus)
}
#endif

#endif // __BLUETOOTH_RFCOMM_H
