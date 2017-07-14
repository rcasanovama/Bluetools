#ifndef _BLUETOOTH_SCANNER_H
#define _BLUETOOTH_SCANNER_H

/**
 * Author:  Raul Casanova Marques
 * Date:    08/07/17
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <stdint.h>
#include <stdbool.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include "bt-types.h"

/**
 * Performs and inquiry scan of bluetooth devices
 *
 * @param adapter local bluetooth device
 * @param devices list of bluetooth devices found with the scan
 * @return number of bluetooth devices found with the scan
 */
uint8_t device_inquiry_scan(struct btd_adapter adapter, struct btd_device** devices);

/**
 * Prints the information of the remote device
 *
 * @param device remote bluetooth device
 */
void device_print_information(struct btd_device device);

/**
 * Reads the name of the remote bluetooth device
 *
 * @param adapter local bluetooth device
 * @param device remote bluetooth device
 * @return true on success, false otherwise
 */
bool device_read_remote_name(struct btd_adapter adapter, struct btd_device* device);

/**
 * Cleans the information of the remote device
 *
 * @param device remote bluetooth device
 */
void device_cleanup(struct btd_device* device);

#ifdef __cplusplus
}
#endif

#endif // _BLUETOOTH_SCANNER_H
