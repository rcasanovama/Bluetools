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

#ifndef __BLUETOOTH_UTILS_H
#define __BLUETOOTH_UTILS_H

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

#endif // __BLUETOOTH_UTILS_H