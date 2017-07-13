#ifndef _BLUETOOTH_TYPES_H
#define _BLUETOOTH_TYPES_H

/**
 * Author:  Raul Casanova Marques
 * Date:    13/07/17
 */

#include <stdint.h>

#include <bluetooth/bluetooth.h>

struct btd_adapter_name
{
	char* name;              /* controller device name */
	char* short_name;        /* controller short name */
};

struct btd_adapter_class
{
	uint8_t cls[3];          /* controller major and minor class */
	uint32_t dev_class;      /* controller class of device */

	uint8_t major_class;     /* configured major class */
	uint8_t minor_class;     /* configured minor class */
};

struct btd_adapter_address
{
	char addr[19];           /* controller plain text address */
	bdaddr_t bdaddr;         /* controller Bluetooth address */
};

struct btd_adapter
{
	uint16_t dev_id;

	struct btd_adapter_name* adapter_name;

	struct btd_adapter_class* adapter_class;

	struct btd_adapter_address* adapter_address;
};

#endif // _BLUETOOTH_TYPES_H
