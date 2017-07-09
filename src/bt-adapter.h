#ifndef _BLUETOOTH_ADAPTER_H
#define _BLUETOOTH_ADAPTER_H

/**
 * Author:  Raul Casanova Marques
 * Date:    08/07/17
 */

#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

int about_local_device(int dev_id, int dev_handle);

#endif // _BLUETOOTH_ADAPTER_H
