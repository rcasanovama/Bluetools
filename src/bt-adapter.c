/**
 * Author:  Raul Casanova Marques
 * Date:    08/07/17
 */
#include "bt-adapter.h"

extern int hci_write_bd_addr(int dd, uint16_t dev_id, char* _addr, uint8_t _transient, uint8_t _reset);

static char* get_minor_device_name(int major, int minor);

static struct btd_adapter* adapter_init(uint16_t dev_id)
{
	struct btd_adapter* adapter;
	int dd;

	// Only to check if the device exists
	dd = hci_open_dev(dev_id);
	if (dd < 0)
	{
		return NULL;
	}
	hci_close_dev(dd);

	adapter = (struct btd_adapter*) malloc(sizeof(struct btd_adapter));
	if (! adapter)
	{
		return NULL;
	}

	adapter->dev_id = (uint16_t) dev_id;
	adapter->adapter_name = adapter_read_local_name(adapter->dev_id);
	assert(adapter->adapter_name != NULL);
	adapter->adapter_class = adapter_read_local_class(adapter->dev_id);
	assert(adapter->adapter_class != NULL);
	adapter->adapter_address = adapter_read_local_address(adapter->dev_id);
	assert(adapter->adapter_address != NULL);
	adapter->adapter_version = adapter_read_local_version(adapter->dev_id);
	assert(adapter->adapter_version != NULL);
	adapter->afh_mode = adapter_read_local_afh_mode(adapter->afh_mode);

	return adapter;
}

struct btd_adapter* adapter_get_default(void)
{
	int32_t dev_id;

	dev_id = hci_get_route(NULL);
	if (dev_id < 0)
	{
		return NULL;
	}

	return adapter_init((uint16_t) dev_id);
}

struct btd_adapter* adapter_find_by_id(uint16_t dev_id)
{
	return adapter_init(dev_id);
}

uint16_t adapter_get_all(struct btd_adapter** adapters)
{
	struct hci_dev_list_req* dev_list_req;
	struct hci_dev_req* dev_req;

	int32_t btd_socket, dev_id;
	uint16_t number_adapters;
	int i;


	btd_socket = socket(AF_BLUETOOTH, SOCK_RAW | SOCK_CLOEXEC, BTPROTO_HCI);
	if (btd_socket < 0)
	{
		return 0;
	}

	dev_list_req = (struct hci_dev_list_req*) malloc(HCI_MAX_DEV * sizeof(*dev_req) + sizeof(*dev_list_req));
	if (! dev_list_req)
	{
		close(btd_socket);
		return 0;
	}

	memset(dev_list_req, 0, HCI_MAX_DEV * sizeof(*dev_req) + sizeof(*dev_list_req));

	dev_list_req->dev_num = HCI_MAX_DEV;
	dev_req = dev_list_req->dev_req;

	if (ioctl(btd_socket, HCIGETDEVLIST, (void*) dev_list_req) < 0)
	{
		free(dev_list_req);
		close(btd_socket);
		return 0;
	}

	(*adapters) = NULL;
	number_adapters = 0;
	for (i = 0; i < dev_list_req->dev_num; i ++, dev_req ++)
	{
		if (hci_test_bit(HCI_UP, &dev_req->dev_opt))
		{
			dev_id = dev_req->dev_id;
			if (dev_id >= 0)
			{
				(*adapters) = (struct btd_adapter*) realloc((*adapters), (number_adapters + 1) * sizeof(struct btd_adapter));
				(*adapters)[number_adapters] = *(adapter_init((uint16_t) dev_id));
				number_adapters ++;
			}
		}
	}

	return number_adapters;
}

uint16_t adapter_get_index(struct btd_adapter adapter)
{
	return adapter.dev_id;
}

void adapter_print_information(struct btd_adapter adapter)
{
	struct hci_dev_info dev_info;
	int dd, err = 0;

	dd = hci_open_dev(adapter.dev_id);
	if (dd < 0)
	{
		perror("HCI device open failed");
		err = 1;
	}

	if (hci_devinfo(adapter.dev_id, &dev_info) < 0)
	{
		perror("Can't get device info");
		err = 1;

		hci_close_dev(dd);
	}

	if (! err)
	{
		adapter_print_name(*adapter.adapter_name);
		fprintf(stdout, "  Type: %s  Bus: %s\n", hci_typetostr((dev_info.type & 0x30) >> 4), hci_bustostr(dev_info.type & 0x0f));
		fprintf(stdout, "\tBD Address: %s  ACL MTU: %d:%d  SCO MTU: %d:%d\n", adapter.adapter_address->addr, dev_info.acl_mtu, dev_info.acl_pkts, dev_info.sco_mtu, dev_info.sco_pkts);

		fprintf(stdout, "\n");
		adapter_print_class(*adapter.adapter_class);

		fprintf(stdout, "\n");
		adapter_print_version(*adapter.adapter_version);

		fprintf(stdout, "\n");
		adapter_print_afh_mode(adapter.afh_mode);

		fprintf(stdout, "\n");
	}
}

struct btd_adapter_name* adapter_read_local_name(uint16_t dev_id)
{
	struct btd_adapter_name* adapter_name;
	struct hci_dev_info dev_info;
	char name[HCI_MAX_NAME_LENGTH];
	int dd;

	dd = hci_open_dev(dev_id);
	if (dd < 0)
	{
		return NULL;
	}

	if (hci_devinfo(dev_id, &dev_info) < 0)
	{
		hci_close_dev(dd);
		return NULL;
	}

	adapter_name = (struct btd_adapter_name*) malloc(sizeof(struct btd_adapter_name));
	if (! adapter_name)
	{
		hci_close_dev(dd);
		return NULL;
	}

	if (hci_read_local_name(dd, sizeof(name), name, 1000) < 0)
	{
		free(adapter_name);
		hci_close_dev(dd);
		return NULL;
	}

	adapter_name->name = strdup(name);
	adapter_name->short_name = strdup(dev_info.name);

	hci_close_dev(dd);

	return adapter_name;
}

bool adapter_write_local_name(uint16_t dev_id, struct btd_adapter_name adapter_name)
{
	int dd;

	dd = hci_open_dev(dev_id);
	if (dd < 0)
	{
		return false;
	}

	if (hci_write_local_name(dd, adapter_name.name, 1000) < 0)
	{
		hci_close_dev(dd);
		return false;
	}

	hci_close_dev(dd);

	return true;
}

void adapter_print_name(struct btd_adapter_name adapter_name)
{
	fprintf(stdout, "%s:\t%s", adapter_name.short_name, adapter_name.name);
}

struct btd_adapter_class* adapter_read_local_class(uint16_t dev_id)
{
	struct btd_adapter_class* adapter_class;
	char dev_class[10];
	uint8_t cls[3];
	int dd;

	dd = hci_open_dev(dev_id);
	if (dd < 0)
	{
		return NULL;
	}

	adapter_class = (struct btd_adapter_class*) malloc(sizeof(struct btd_adapter_class));
	if (! adapter_class)
	{
		hci_close_dev(dd);
		return NULL;
	}

	if (hci_read_class_of_dev(dd, cls, 1000) < 0)
	{
		free(adapter_class);
		hci_close_dev(dd);
		return NULL;
	}

	adapter_class->cls[2] = cls[2];
	adapter_class->cls[1] = cls[1];
	adapter_class->cls[0] = cls[0];

	snprintf(dev_class, 10, "%02x%02x%02x", cls[2], cls[1], cls[0]);
	adapter_class->dev_class = (uint32_t) strtoul(dev_class, NULL, 16);

	adapter_class->major_class = cls[1] & ((uint8_t) 0x1f);
	adapter_class->minor_class = cls[0] >> 2;

	hci_close_dev(dd);

	return adapter_class;
}

bool adapter_write_local_class(uint16_t dev_id, struct btd_adapter_class adapter_class)
{
	int dd;

	dd = hci_open_dev(dev_id);
	if (dd < 0)
	{
		return false;
	}

	if (hci_write_class_of_dev(dd, adapter_class.dev_class, 1000) < 0)
	{
		hci_close_dev(dd);
		return false;
	}

	hci_close_dev(dd);

	return true;
}

void adapter_print_class(struct btd_adapter_class adapter_class)
{
	static const char* services[] = {"Positioning", "Networking", "Rendering", "Capturing", "Object Transfer", "Audio", "Telephony", "Information"};
	static const char* major_devices[] = {"Miscellaneous", "Computer", "Phone", "LAN Access", "Audio/Video", "Peripheral", "Imaging", "Uncategorized"};

	bool first;
	unsigned int i;

	fprintf(stdout, "\tDevice class:\t\t0x%02x%02x%02x\n", adapter_class.cls[2], adapter_class.cls[1], adapter_class.cls[0]);
	fprintf(stdout, "\tService classes:\t");
	if (adapter_class.cls[2])
	{
		first = true;
		for (i = 0; i < (sizeof(services) / sizeof(*services)); i ++)
		{
			if (adapter_class.cls[2] & (1 << i))
			{
				if (! first)
				{
					fprintf(stdout, ", ");
				}
				fprintf(stdout, "%s", services[i]);
				first = false;
			}
		}
		fprintf(stdout, "\n");
	}
	else
	{
		fprintf(stdout, "Unspecified\n");
	}

	fprintf(stdout, "\tClass description:\t");
	if ((adapter_class.major_class) >= sizeof(major_devices) / sizeof(*major_devices))
	{
		fprintf(stdout, "Invalid device class!\n");
	}
	else
	{
		fprintf(stdout, "%s, %s\n", major_devices[adapter_class.major_class], get_minor_device_name(adapter_class.major_class, adapter_class.minor_class));
	}
}

struct btd_adapter_address* adapter_read_local_address(uint16_t dev_id)
{
	struct btd_adapter_address* adapter_address;
	struct hci_dev_info dev_info;
	int dd;

	dd = hci_open_dev(dev_id);
	if (dd < 0)
	{
		return NULL;
	}

	if (hci_devinfo(dev_id, &dev_info) < 0)
	{
		hci_close_dev(dd);
		return NULL;
	}

	adapter_address = (struct btd_adapter_address*) malloc(sizeof(struct btd_adapter_address));
	if (! adapter_address)
	{
		hci_close_dev(dd);
		return NULL;
	}

	adapter_address->bdaddr = dev_info.bdaddr;
	ba2str(&dev_info.bdaddr, adapter_address->addr);

	hci_close_dev(dd);

	return adapter_address;
}

bool adapter_write_local_address(uint16_t dev_id, struct btd_adapter_address adapter_address)
{
	int dd;

	dd = hci_open_dev(dev_id);
	if (dd < 0)
	{
		return false;
	}

	if (hci_write_bd_addr(dd, dev_id, adapter_address.addr, 0, 0) < 0)
	{
		hci_close_dev(dd);
		return false;
	}

	hci_close_dev(dd);

	return true;
}

void adapter_print_address(struct btd_adapter_address adapter_address)
{
	fprintf(stdout, "\tBD Address: %s", adapter_address.addr);
}

struct btd_adapter_version* adapter_read_local_version(uint16_t dev_id)
{
	struct btd_adapter_version* adapter_version;
	struct hci_dev_info dev_info;
	struct hci_version ver;
	int dd;

	dd = hci_open_dev(dev_id);
	if (dd < 0)
	{
		return NULL;
	}

	if (hci_devinfo(dev_id, &dev_info) < 0)
	{
		hci_close_dev(dd);
		return NULL;
	}

	adapter_version = (struct btd_adapter_version*) malloc(sizeof(struct btd_adapter_version));
	if (! adapter_version)
	{
		hci_close_dev(dd);
		return NULL;
	}

	if (hci_read_local_version(dd, &ver, 1000) < 0)
	{
		free(adapter_version);
		hci_close_dev(dd);
		return NULL;
	}

	adapter_version->ver = ver;
	adapter_version->hci_ver = hci_vertostr(ver.hci_ver);
	if (((dev_info.type & 0x30) >> 4) == HCI_BREDR)
	{
		adapter_version->lmp_ver = lmp_vertostr(ver.lmp_ver);
	}
	else
	{
		adapter_version->lmp_ver = pal_vertostr(ver.lmp_ver);
	}
	adapter_version->type = dev_info.type;

	return adapter_version;
}

void adapter_print_version(struct btd_adapter_version adapter_version)
{
	fprintf(stdout, "\tHCI Version: %s (0x%x)  Revision: 0x%x\n"
			"\t%s Version: %s (0x%x)  Subversion: 0x%x\n"
			"\tManufacturer: %s (%d)\n", adapter_version.hci_ver ? adapter_version.hci_ver : "n/a", adapter_version.ver.hci_ver, adapter_version.ver.hci_rev, (((adapter_version.type & 0x30) >> 4) == HCI_BREDR) ? "LMP" : "PAL", adapter_version.lmp_ver ? adapter_version.lmp_ver : "n/a", adapter_version.ver.lmp_ver, adapter_version.ver.lmp_subver, bt_compidtostr(adapter_version.ver.manufacturer), adapter_version.ver.manufacturer);
}

uint8_t adapter_read_local_afh_mode(uint16_t dev_id)
{
	uint8_t afh_mode;
	int dd;

	dd = hci_open_dev(dev_id);
	if (dd < 0)
	{
		return 0;
	}

	if (hci_read_afh_mode(dd, &afh_mode, 1000) < 0)
	{
		hci_close_dev(dd);
		return 0;
	}

	return afh_mode;
}

void adapter_print_afh_mode(uint8_t afh_mode)
{
	fprintf(stdout, "\tAFH mode: %s", afh_mode == 1 ? "Enabled" : "Disabled");
}

void adapter_cleanup(struct btd_adapter* adapter)
{
	// free adapter_name
	if (adapter->adapter_name)
	{
		if (adapter->adapter_name->name)
		{
			free(adapter->adapter_name->name);
			adapter->adapter_name->name = NULL;
		}
		if (adapter->adapter_name->short_name)
		{
			free(adapter->adapter_name->short_name);
			adapter->adapter_name->short_name = NULL;
		}
		free(adapter->adapter_name);
		adapter->adapter_name = NULL;
	}
	// free adapter_class
	if (adapter->adapter_class)
	{
		free(adapter->adapter_class);
		adapter->adapter_class = NULL;
	}
	// free adapter_address
	if (adapter->adapter_address)
	{
		free(adapter->adapter_address);
		adapter->adapter_address = NULL;
	}
	// free adapter_version
	if (adapter->adapter_version)
	{
		if (adapter->adapter_version->hci_ver)
		{
			free(adapter->adapter_version->hci_ver);
			adapter->adapter_version->hci_ver = NULL;
		}
		if (adapter->adapter_version->lmp_ver)
		{
			free(adapter->adapter_version->lmp_ver);
			adapter->adapter_version->lmp_ver = NULL;
		}
		free(adapter->adapter_version);
		adapter->adapter_version = NULL;
	}
}

static char* get_minor_device_name(int major, int minor)
{
	switch (major)
	{
		case 0: // misc
		{
			return "";
		}
		case 1: // computer
		{
			switch (minor)
			{
				case 0:
					return "Uncategorized";
				case 1:
					return "Desktop workstation";
				case 2:
					return "Server";
				case 3:
					return "Laptop";
				case 4:
					return "Handheld";
				case 5:
					return "Palm";
				case 6:
					return "Wearable";
				default:
					break;
			}

			break;
		}
		case 2: // phone
		{
			switch (minor)
			{
				case 0:
					return "Uncategorized";
				case 1:
					return "Cellular";
				case 2:
					return "Cordless";
				case 3:
					return "Smart phone";
				case 4:
					return "Wired modem or voice gateway";
				case 5:
					return "Common ISDN Access";
				case 6:
					return "Sim Card Reader";
				default:
					break;
			}

			break;
		}
		case 3: // lan access
		{
			if (minor == 0)
			{
				return "Uncategorized";
			}
			switch (minor / 8)
			{
				case 0:
					return "Fully available";
				case 1:
					return "1-17% utilized";
				case 2:
					return "17-33% utilized";
				case 3:
					return "33-50% utilized";
				case 4:
					return "50-67% utilized";
				case 5:
					return "67-83% utilized";
				case 6:
					return "83-99% utilized";
				case 7:
					return "No service available";
				default:
					break;
			}

			break;
		}
		case 4: // audio/video
		{
			switch (minor)
			{
				case 0:
					return "Uncategorized";
				case 1:
					return "Device conforms to the Headset profile";
				case 2:
					return "Hands-free";
					// 3 is reserved
				case 4:
					return "Microphone";
				case 5:
					return "Loudspeaker";
				case 6:
					return "Headphones";
				case 7:
					return "Portable Audio";
				case 8:
					return "Car Audio";
				case 9:
					return "Set-top box";
				case 10:
					return "HiFi Audio Device";
				case 11:
					return "VCR";
				case 12:
					return "Video Camera";
				case 13:
					return "Camcorder";
				case 14:
					return "Video Monitor";
				case 15:
					return "Video Display and Loudspeaker";
				case 16:
					return "Video Conferencing";
					// 17 is reserved
				case 18:
					return "Gaming/Toy";
				default:
					break;
			}

			break;
		}
		case 5: // peripheral
		{
			static char cls_str[48];

			cls_str[0] = '\0';

			switch (minor & 48)
			{
				case 16:
					strncpy(cls_str, "Keyboard", sizeof(cls_str));
					break;
				case 32:
					strncpy(cls_str, "Pointing device", sizeof(cls_str));
					break;
				case 48:
					strncpy(cls_str, "Combo keyboard/pointing device", sizeof(cls_str));
					break;
				default:
					break;
			}
			if ((minor & 15) && (strlen(cls_str) > 0))
			{
				strcat(cls_str, "/");
			}

			switch (minor & 15)
			{
				case 0:
					break;
				case 1:
					strncat(cls_str, "Joystick", sizeof(cls_str) - strlen(cls_str));
					break;
				case 2:
					strncat(cls_str, "Gamepad", sizeof(cls_str) - strlen(cls_str));
					break;
				case 3:
					strncat(cls_str, "Remote control", sizeof(cls_str) - strlen(cls_str));
					break;
				case 4:
					strncat(cls_str, "Sensing device", sizeof(cls_str) - strlen(cls_str));
					break;
				case 5:
					strncat(cls_str, "Digitizer tablet", sizeof(cls_str) - strlen(cls_str));
					break;
				case 6:
					strncat(cls_str, "Card reader", sizeof(cls_str) - strlen(cls_str));
					break;
				default:
					strncat(cls_str, "(reserved)", sizeof(cls_str) - strlen(cls_str));
					break;
			}
			if (strlen(cls_str) > 0)
			{
				return cls_str;
			}

			break;
		}
		case 6: // imaging
		{
			if (minor & 4)
			{
				return "Display";
			}
			if (minor & 8)
			{
				return "Camera";
			}
			if (minor & 16)
			{
				return "Scanner";
			}
			if (minor & 32)
			{
				return "Printer";
			}

			break;
		}
		case 7: // wearable
		{
			switch (minor)
			{
				case 1:
					return "Wrist Watch";
				case 2:
					return "Pager";
				case 3:
					return "Jacket";
				case 4:
					return "Helmet";
				case 5:
					return "Glasses";
				default:
					break;
			}

			break;
		}
		case 8: // toy
		{
			switch (minor)
			{
				case 1:
					return "Robot";
				case 2:
					return "Vehicle";
				case 3:
					return "Doll / Action Figure";
				case 4:
					return "Controller";
				case 5:
					return "Game";
				default:
					break;
			}

			break;
		}
		case 63: // uncategorized
		{
			return "";
		}
		default:
		{
			break;
		}
	}
	return "Unknown (reserved) minor device class";
}