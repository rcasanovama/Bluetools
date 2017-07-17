/**
 * Author:  Raul Casanova Marques
 * Date:    15/07/17
 */
#include "bt-utils.h"

struct btd_adapter_name* str_to_adapter_name_t(char* _name)
{
	struct btd_adapter_name* adapter_name;

	if (! _name)
	{
		return NULL;
	}

	adapter_name = (struct btd_adapter_name*) malloc(sizeof(struct btd_adapter_name));
	if (! adapter_name)
	{
		return NULL;
	}

	adapter_name->name = strdup(_name);
	adapter_name->short_name = NULL;

	return adapter_name;
}

struct btd_adapter_class* str_to_adapter_class_t(char* _class)
{
	struct btd_adapter_class* adapter_class;
	char cls_t[3], class_t[7];
	size_t length;

	if (! _class)
	{
		return NULL;
	}

	adapter_class = (struct btd_adapter_class*) malloc(sizeof(struct btd_adapter_class));
	if (! adapter_class)
	{
		return NULL;
	}

	length = strlen(_class);
	if (length > 6)
	{
		free(adapter_class);
		return NULL;
	}

	// Ex: 123 -> 000123 (left padding)
	memset(class_t, '0', 7);
	snprintf(&class_t[6 - length], 7, "%s", _class);

	// cls[2]
	snprintf(cls_t, 3, "%c%c", class_t[0], class_t[1]);
	adapter_class->cls[2] = (uint8_t) strtoul(cls_t, NULL, 16);
	// cls[1]
	snprintf(cls_t, 3, "%c%c", class_t[2], class_t[3]);
	adapter_class->cls[1] = (uint8_t) strtoul(cls_t, NULL, 16);
	// cls[0]
	snprintf(cls_t, 3, "%c%c", class_t[4], class_t[5]);
	adapter_class->cls[0] = (uint8_t) strtoul(cls_t, NULL, 16);

	adapter_class->dev_class = (uint32_t) strtoul(_class, NULL, 16);

	adapter_class->major_class = adapter_class->cls[1] & ((uint8_t) 0x1f);
	adapter_class->minor_class = adapter_class->cls[0] >> 2;

	return adapter_class;
}

struct btd_adapter_class* cls_to_adapter_class_t(uint8_t _cls[3])
{
	struct btd_adapter_class* adapter_class;
	char dev_class[10];

	adapter_class = (struct btd_adapter_class*) malloc(sizeof(struct btd_adapter_class));
	if (! adapter_class)
	{
		return NULL;
	}

	adapter_class->cls[2] = _cls[2];
	adapter_class->cls[1] = _cls[1];
	adapter_class->cls[0] = _cls[0];

	snprintf(dev_class, 10, "%02x%02x%02x", _cls[2], _cls[1], _cls[0]);
	adapter_class->dev_class = (uint32_t) strtoul(dev_class, NULL, 16);

	adapter_class->major_class = _cls[1] & ((uint8_t) 0x1f);
	adapter_class->minor_class = _cls[0] >> 2;

	return adapter_class;
}

struct btd_adapter_address* str_to_adapter_address_t(char* _address)
{
	struct btd_adapter_address* adapter_address;

	if (! _address)
	{
		return NULL;
	}

	adapter_address = (struct btd_adapter_address*) malloc(sizeof(struct btd_adapter_address));
	if (! adapter_address)
	{
		return NULL;
	}

	strncpy(adapter_address->addr, _address, 19);
	str2ba(adapter_address->addr, &adapter_address->bdaddr);

	return adapter_address;
}