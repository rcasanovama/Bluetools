/**
 * Author:  Raul Casanova Marques
 * Date:    08/07/17
 */
#include "bluescan.h"

/**
 * Performs and inquiry scan of bluetooth devices
 *
 * @param dev_id identifier of the bluetooth device
 * @param dev_handle handle of the bluetooth connection
 * @param bluetooth_devices list of bluetooth devices found with the scan
 * @return number of bluetooth devices found with the scan
 */
uint8_t inquiry_scan(int dev_id, int dev_handle, T_BLUETOOTH_DEVICE** bluetooth_devices)
{
	inquiry_info* ii = NULL;
	int max_rsp, num_rsp;
	int len, flags;
	int i;

	len = 8;
	max_rsp = 255;
	flags = IREQ_CACHE_FLUSH;
	ii = (inquiry_info*) malloc(max_rsp * sizeof(inquiry_info));

	num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
	if (num_rsp < 0)
	{
		perror("hci_inquiry");
	}

	(*bluetooth_devices) = (T_BLUETOOTH_DEVICE*) malloc(num_rsp * sizeof(T_BLUETOOTH_DEVICE));
	for (i = 0; i < num_rsp; i ++)
	{
		// bdaddr struct
		(*bluetooth_devices)[i].bdaddr = (ii + i)->bdaddr;
		// bdaddr to plain text
		ba2str(&(ii + i)->bdaddr, (*bluetooth_devices)[i].bdaddr_str);
		// bluetooth name
		query_bt_name(dev_handle, &(*bluetooth_devices)[i]);
		// dev class
		(*bluetooth_devices)[i].dev_class[2] = (ii + i)->dev_class[2];
		(*bluetooth_devices)[i].dev_class[1] = (ii + i)->dev_class[1];
		(*bluetooth_devices)[i].dev_class[0] = (ii + i)->dev_class[0];
		// clock offset
		(*bluetooth_devices)[i].clock_offset = (ii + i)->clock_offset;
	}

	free(ii);

	return (uint8_t) num_rsp;
}

/**
 * Queries the name of the remote bluetooth device
 *
 * @param dev_handle handle of the bluetooth connection
 * @param bluetooth_device bluetooth device
 */
void query_bt_name(int dev_handle, T_BLUETOOTH_DEVICE* bluetooth_device)
{
	if (hci_read_remote_name(dev_handle, &bluetooth_device->bdaddr, sizeof(bluetooth_device->name), bluetooth_device->name, 0) < 0)
	{
		strcpy(bluetooth_device->name, "[unknown]");
	}
}