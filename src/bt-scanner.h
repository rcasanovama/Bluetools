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

#ifndef __BLUETOOTH_SCANNER_H
#define __BLUETOOTH_SCANNER_H

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
extern uint8_t device_inquiry_scan(struct btd_adapter adapter, struct btd_device** devices);

/**
 * Prints the information of the remote device
 *
 * @param device remote bluetooth device
 */
extern void device_print_information(struct btd_device device);

/**
 * Reads the name of the remote bluetooth device
 *
 * @param adapter local bluetooth device
 * @param device remote bluetooth device
 * @return true on success, false otherwise
 */
extern bool device_read_remote_name(struct btd_adapter adapter, struct btd_device* device);

/**
 * Cleans the information of the remote device
 *
 * @param device remote bluetooth device
 */
extern void device_cleanup(struct btd_device* device);

#ifdef __cplusplus
}
#endif

#endif // __BLUETOOTH_SCANNER_H
