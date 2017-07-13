#ifndef _BLUETOOTH_ADAPTER_H
#define _BLUETOOTH_ADAPTER_H

/**
 * Author:  Raul Casanova Marques
 * Date:    08/07/17
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <errno.h>
#include <sys/ioctl.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include "bt-types.h"

struct btd_adapter* adapter_get_default(void);

uint16_t adapter_get_all(struct btd_adapter** adapters);

uint16_t adapter_get_index(struct btd_adapter* adapter);

struct btd_adapter_name* adapter_read_local_name(uint16_t dev_id);

bool adapter_write_local_name(uint16_t dev_id, struct btd_adapter_name adapter_name);

void adapter_print_name(struct btd_adapter_name adapter_name);

struct btd_adapter_class* adapter_read_local_class(uint16_t dev_id);

bool adapter_write_local_class(uint16_t dev_id, struct btd_adapter_class adapter_class);

void adapter_print_class(struct btd_adapter_class adapter_class);

struct btd_adapter_address* adapter_read_local_address(uint16_t dev_id);

bool adapter_write_local_address(uint16_t dev_id, struct btd_adapter_address adapter_address);

void adapter_print_address(struct btd_adapter_address adapter_address);

void adapter_cleanup(struct btd_adapter* adapter);


int about_local_device(int dev_handle, int dev_id, long arg);

#endif // _BLUETOOTH_ADAPTER_H
