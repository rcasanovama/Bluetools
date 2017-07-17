#ifndef _BLUETOOTH_AGENT_H
#define _BLUETOOTH_AGENT_H

/**
 * Author:  Raul Casanova Marques
 * Date:    16/07/17
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <unistd.h>

#include <stdint.h>
#include <assert.h>

//#include <bluetooth/sdp.h>
//#include <bluetooth/sdp_lib.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/l2cap.h>

int agent_init_rfcomm_server(uint8_t channel);

int agent_init_rfcomm_client(uint8_t channel, char* address);

int agent_init_l2cap_server(unsigned short l2_psm);

int agent_init_l2cap_client(unsigned short l2_psm, char* address);

ssize_t agent_read_data(int client, char* buf, size_t buf_size);

ssize_t agent_write_data(int client, char* buf);

void agent_cleanup(int fd);

#ifdef __cplusplus
}
#endif

#endif // _BLUETOOTH_AGENT_H