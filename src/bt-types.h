/**
 *
 *  Copyright (C) 2017  Raul Casanova Marques
 *
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __BLUETOOTH_TYPES_H
#define __BLUETOOTH_TYPES_H

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

#endif // __BLUETOOTH_TYPES_H
