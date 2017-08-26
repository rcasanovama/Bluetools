#ifndef _BLUETOOTH_ADAPTER_H
#define _BLUETOOTH_ADAPTER_H

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

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include <errno.h>
#include <sys/ioctl.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include "bt-types.h"

/**
 * Gets the default bluetooth adapter.
 *
 * @return default bluetooth adapter
 */
extern struct btd_adapter* adapter_get_default(void);

/**
 * Finds the adapter identified by dev_id.
 *
 * @param dev_id identifier of the adapter
 * @return adapter or null if not exists
 */
extern struct btd_adapter* adapter_find_by_id(uint16_t dev_id);

/**
 * Gets all bluetooth adapters.
 *
 * @param adapters list of adapters
 * @return number of adapters in the list
 */
extern uint16_t adapter_get_all(struct btd_adapter** adapters);

/**
 * Gets the index of the specified adapter.
 *
 * @param adapter bluetooth adapter
 * @return index of the adapter
 */
extern uint16_t adapter_get_index(struct btd_adapter adapter);

/**
 * Prints the information of the bluetooth adapter.
 *
 * @param adapter bluetooth adapter
 */
extern void adapter_print_information(struct btd_adapter adapter);

/**
 * Reads the local name of the bluetooth adapter identified by dev_id.
 *
 * @param dev_id identifier of the adapter
 * @return name as btd_adapter_name format
 */
extern struct btd_adapter_name* adapter_read_local_name(uint16_t dev_id);

/**
 * Writes the local name of the bluetooth adapter.
 *
 * @param dev_id identifier of the adapter
 * @param adapter_name new name of the adapter
 * @return true on success, false otherwise
 */
extern bool adapter_write_local_name(uint16_t dev_id, struct btd_adapter_name adapter_name);

/**
 * Prints the local name of the bluetooth adapter.
 *
 * @param adapter_name name of the bluetooth adapter
 */
extern void adapter_print_name(struct btd_adapter_name adapter_name);

/**
 * Reads the local class of the bluetooth adapter identified by dev_id.
 *
 * @param dev_id identifier of the adapter
 * @return class as btd_adapter_class format
 */
extern struct btd_adapter_class* adapter_read_local_class(uint16_t dev_id);

/**
 * Writes the local class of the bluetooth adapter.
 *
 * @param dev_id identifier of the adapter
 * @param adapter_class new class of the adapter
 * @return true on success, false otherwise
 */
extern bool adapter_write_local_class(uint16_t dev_id, struct btd_adapter_class adapter_class);

/**
 * Prints the local class of the bluetooth adapter.
 *
 * @param adapter_class class of the bluetooth adapter
 */
extern void adapter_print_class(struct btd_adapter_class adapter_class);

/**
 * Reads the local address of the bluetooth adapter identified by dev_id.
 *
 * @param dev_id identifier of the adapter
 * @return address as btd_adapter_address format
 */
extern struct btd_adapter_address* adapter_read_local_address(uint16_t dev_id);

/**
 * Writes the local address of the bluetooth adapter.
 *
 * @param dev_id identifier of the adapter
 * @param adapter_address new address of the adapter
 * @return true on success, false otherwise
 */
extern bool adapter_write_local_address(uint16_t dev_id, struct btd_adapter_address adapter_address);

/**
 * Prints the local address of the bluetooth adapter.
 *
 * @param adapter_address address of the bluetooth adapter
 */
extern void adapter_print_address(struct btd_adapter_address adapter_address);

/**
 * Reads the local version of the bluetooth adapter identified by dev_id.
 *
 * @param dev_id identifier of the adapter
 * @return version as btd_adapter_version format
 */
extern struct btd_adapter_version* adapter_read_local_version(uint16_t dev_id);

/**
 * Prints the local version of the bluetooth adapter.
 *
 * @param adapter_version version of the bluetooth adapter
 */
extern void adapter_print_version(struct btd_adapter_version adapter_version);

/**
 * Reads the afh mode of the bluetooth adapter identified by dev_id.
 *
 * @param dev_id identifier of the adapter
 * @return afh mode of the bluetooth adapter
 */
extern uint8_t adapter_read_local_afh_mode(uint16_t dev_id);

/**
 * Prints the afh mode of the bluetooth adapter.
 *
 * @param afh_mode afh mode of the bluetooth adapter
 */
extern void adapter_print_afh_mode(uint8_t afh_mode);

/**
 * Cleans the bluetooth adapter.
 *
 * @param adapter bluetooth adapter
 */
extern void adapter_cleanup(struct btd_adapter* adapter);

#ifdef __cplusplus
}
#endif

#endif // _BLUETOOTH_ADAPTER_H
