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

struct btd_adapter_name* str_to_adapter_name_t(char* _name);

struct btd_adapter_class* str_to_adapter_class_t(char* _class);

struct btd_adapter_address* str_to_adapter_address_t(char* _address);

#ifdef __cplusplus
}
#endif

#endif // _BLUETOOTH_UTILS_H