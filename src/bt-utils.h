#ifndef _BLUETOOTH_UTILS_H
#define _BLUETOOTH_UTILS_H

/**
 * Author:  Raul Casanova Marques
 * Date:    15/07/17
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "bt-types.h"

/**
 * Dumps the name into an adapter_name_t format.
 *
 * @param _name name of the adapter
 * @return name into an adapter_name_t format
 */
extern struct btd_adapter_name* str_to_adapter_name_t(char* _name);

/**
 * Dumps the class into an adapter_class_t format.
 *
 * @param _class class of the adapter
 * @return class into an adapter_class_t format
 */
extern struct btd_adapter_class* str_to_adapter_class_t(char* _class);

/**
 * Dumps the class into an adapter_class_t format.
 *
 * @param _cls class of the adapter
 * @return class into an adapter_class_t format
 */
extern struct btd_adapter_class* cls_to_adapter_class_t(uint8_t _cls[3]);

/**
 * Dumps the address into an adapter_address_t format.
 *
 * @param _address address of the adapter
 * @return address into an adapter_address_t format
 */
extern struct btd_adapter_address* str_to_adapter_address_t(char* _address);

#ifdef __cplusplus
}
#endif

#endif // _BLUETOOTH_UTILS_H