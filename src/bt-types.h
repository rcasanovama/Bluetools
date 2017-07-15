#ifndef _BLUETOOTH_TYPES_H
#define _BLUETOOTH_TYPES_H

/**
 * Author:  Raul Casanova Marques
 * Date:    13/07/17
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

struct btd_adapter_name
{
	char* name;                  /* controller device name */
	char* short_name;            /* controller short name */
};

struct btd_adapter_class
{
	uint8_t cls[3];              /* controller major and minor class */
	uint32_t dev_class;          /* controller class of device */

	uint8_t major_class;         /* configured major class */
	uint8_t minor_class;         /* configured minor class */
};

struct btd_adapter_address
{
	char addr[19];               /* controller plain text address */
	bdaddr_t bdaddr;             /* controller Bluetooth address */
};

struct btd_adapter_version
{
	struct hci_version ver;      /* controller version */
	char* hci_ver, * lmp_ver;    /* hci and lmp version */
	uint8_t type;                /* controller type */
};

struct btd_adapter
{
	uint16_t dev_id;

	struct btd_adapter_name* adapter_name;
	struct btd_adapter_class* adapter_class;
	struct btd_adapter_address* adapter_address;
	struct btd_adapter_version* adapter_version;

	uint8_t afh_mode;
};

struct btd_device_class
{
	uint8_t cls[3];              /* controller major and minor class */
	uint32_t dev_class;          /* controller class of device */

	uint8_t major_class;         /* configured major class */
	uint8_t minor_class;         /* configured minor class */
};

struct btd_device_address
{
	char addr[19];               /* controller plain text address */
	bdaddr_t bdaddr;             /* controller Bluetooth address */
};

struct btd_device
{
	char* name;                  /* controller device name */

	struct btd_device_class* device_class;
	struct btd_device_address* device_address;

	uint16_t clock_offset;       /* controller clock offset */
};

#ifdef __cplusplus
}
#endif

#endif // _BLUETOOTH_TYPES_H
