#ifndef _BLUESCAN_H
#define _BLUESCAN_H

/**
 * Author:  Raul Casanova Marques
 * Date:    08/07/17
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <stdint.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

typedef struct
{
	bdaddr_t bdaddr;
	char bdaddr_str[19];
	char name[HCI_MAX_NAME_LENGTH];

	uint8_t dev_class[3];
	uint16_t clock_offset;

} T_BLUETOOTH_DEVICE;

/**
 * Performs and inquiry scan of bluetooth devices
 *
 * @param dev_id identifier of the bluetooth device
 * @param dev_handle handle of the bluetooth connection
 * @param bluetooth_devices list of bluetooth devices found with the scan
 * @return number of bluetooth devices found with the scan
 */
uint8_t inquiry_scan(int dev_id, int dev_handle, T_BLUETOOTH_DEVICE** bluetooth_devices);

/**
 * Queries the name of the remote bluetooth device
 *
 * @param dev_handle handle of the bluetooth connection
 * @param bluetooth_device bluetooth device
 */
void query_bt_name(int dev_handle, T_BLUETOOTH_DEVICE* bluetooth_device);

#endif // _BLUESCAN_H
