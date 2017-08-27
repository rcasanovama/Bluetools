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

#include "bt-scanner.h"

static struct btd_device* device_init(struct btd_adapter adapter, uint8_t cls[3], bdaddr_t bdaddr, uint16_t clock_offset)
{
	struct btd_device* device;
	char dev_class[10];

	device = (struct btd_device*) malloc(sizeof(struct btd_device));
	if (! device)
	{
		return NULL;
	}

	device->device_class = (struct btd_device_class*) malloc(sizeof(struct btd_device_class));
	if (! device->device_class)
	{
		free(device);
		return NULL;
	}

	device->device_class->cls[2] = cls[2];
	device->device_class->cls[1] = cls[1];
	device->device_class->cls[0] = cls[0];

	snprintf(dev_class, 10, "%02x%02x%02x", cls[2], cls[1], cls[0]);
	device->device_class->dev_class = (uint32_t) strtoul(dev_class, NULL, 16);

	device->device_class->major_class = cls[1] & ((uint8_t) 0x1f);
	device->device_class->minor_class = cls[0] >> 2;

	device->device_address = (struct btd_device_address*) malloc(sizeof(struct btd_device_address));
	if (! device->device_address)
	{
		free(device->device_class);
		free(device);
		return NULL;
	}

	device->device_address->bdaddr = bdaddr;
	ba2str(&bdaddr, device->device_address->addr);

	device_read_remote_name(adapter, device);
	device->clock_offset = clock_offset;

	return device;
}

/**
 * Performs and inquiry scan of bluetooth devices
 *
 * @param adapter local bluetooth device
 * @param devices list of bluetooth devices found with the scan
 * @return number of bluetooth devices found with the scan
 */
uint8_t device_inquiry_scan(struct btd_adapter adapter, struct btd_device** devices)
{
	inquiry_info* ii = NULL;
	int max_rsp, num_rsp;
	int len, flags;
	int i;

	len = 8;
	max_rsp = 255;
	flags = IREQ_CACHE_FLUSH;
	ii = (inquiry_info*) malloc(max_rsp * sizeof(inquiry_info));
	if (! ii)
	{
		perror("malloc (ii)");
		return 0;
	}

	num_rsp = hci_inquiry(adapter.dev_id, len, max_rsp, NULL, &ii, flags);
	if (num_rsp < 0)
	{
		perror("hci_inquiry");

		free(ii);
		return 0;
	}

	(*devices) = (struct btd_device*) malloc(num_rsp * sizeof(struct btd_device));
	if (! (*devices))
	{
		perror("malloc (*devices)");

		free(ii);
		return 0;
	}

	for (i = 0; i < num_rsp; i ++)
	{
		(*devices)[i] = *device_init(adapter, (ii + i)->dev_class, (ii + i)->bdaddr, (ii + i)->clock_offset);
	}

	free(ii);

	return (uint8_t) num_rsp;
}

/**
 * Prints the information of the remote device
 *
 * @param device remote bluetooth device
 */
void device_print_information(struct btd_device device)
{
	fprintf(stdout, "%s\t0x%2.2x%2.2x%2.2x\t%-20s\t(clk: 0x%4.4x)\n", device.device_address->addr, device.device_class->cls[2], device.device_class->cls[1], device.device_class->cls[0], device.name, btohs(device.clock_offset));
}

/**
 * Reads the name of the remote bluetooth device
 *
 * @param adapter local bluetooth device
 * @param device remote bluetooth device
 * @return true on success, false otherwise
 */
bool device_read_remote_name(struct btd_adapter adapter, struct btd_device* device)
{
	char name[HCI_MAX_NAME_LENGTH] = {0};
	int dd, err = 0;

	dd = hci_open_dev(adapter.dev_id);
	if (dd < 0)
	{
		return false;
	}

	if (hci_read_remote_name(dd, &device->device_address->bdaddr, sizeof(name), name, 0) < 0)
	{
		strcpy(name, "[unknown]");
		err = 1;
	}

	device->name = strdup(name);

	hci_close_dev(dd);

	return ! err ? true : false;
}

/**
 * Cleans the information of the remote device
 *
 * @param device remote bluetooth device
 */
void device_cleanup(struct btd_device* device)
{
	// free name
	if (device->name)
	{
		free(device->name);
		device->name = NULL;
	}
	// free device_class
	if (device->device_class)
	{
		free(device->device_class);
		device->device_class = NULL;
	}
	// free device_address
	if (device->device_address)
	{
		free(device->device_address);
		device->device_address = NULL;
	}
}