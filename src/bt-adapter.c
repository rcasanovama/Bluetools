/**
 * Author:  Raul Casanova Marques
 * Date:    08/07/17
 */
#include "bt-adapter.h"

struct btd_adapter* adapter_get_default(void)
{
	struct btd_adapter* adapter;
	int32_t dev_id;

	dev_id = hci_get_route(NULL);
	if (dev_id < 0)
	{
		return NULL;
	}

	adapter = (struct btd_adapter*) malloc(sizeof(struct btd_adapter));

	adapter->dev_id = (uint16_t) dev_id;
	adapter->adapter_name = adapter_read_local_name(adapter->dev_id);
	adapter->adapter_class = adapter_read_local_class(adapter->dev_id);
	adapter->adapter_address = adapter_read_local_address(adapter->dev_id);

	return adapter;
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
				(*adapters) = (struct btd_adapter*) realloc((*adapters), (number_adapters + 1) * sizeof(uint16_t));

				(*adapters)[number_adapters].dev_id = (uint16_t) dev_id;
				(*adapters)[number_adapters].adapter_name = adapter_read_local_name((*adapters)[number_adapters].dev_id);
				(*adapters)[number_adapters].adapter_class = adapter_read_local_class((*adapters)[number_adapters].dev_id);
				(*adapters)[number_adapters].adapter_address = adapter_read_local_address((*adapters)[number_adapters].dev_id);

				number_adapters ++;
			}
		}
	}

	return number_adapters;
}

uint16_t adapter_get_index(struct btd_adapter* adapter)
{
	return adapter->dev_id;
}

struct btd_adapter_name* adapter_read_local_name(uint16_t dev_id)
{
	struct btd_adapter_name* adapter_name;
	char name[HCI_MAX_NAME_LENGTH];
	int dd;

	dd = hci_open_dev(dev_id);
	if (dd < 0)
	{
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

	adapter_name->short_name = NULL;
	adapter_name->name = strdup(name);

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
	return false;
}

void adapter_print_address(struct btd_adapter_address adapter_address)
{

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
}


static int read_class(int dev_id, int dev_handle);

static int read_version(int dev_id, int dev_handle, struct hci_dev_info di);

static int read_afh_mode(int dev_id, int dev_handle);

static char* get_minor_device_name(int major, int minor);

int about_local_device(int dev_handle, int dev_id, long arg)
{
	struct hci_dev_info di = {.dev_id = (uint16_t) dev_id};
	char addr[19] = {0};

	dev_handle = hci_open_dev(dev_id);
	if (dev_handle < 0)
	{
		perror("opening socket");
		return 1;
	}

	if (hci_devinfo(dev_id, &di) < 0)
	{
		perror("Can't get device info");
		return 2;
	}

	ba2str(&di.bdaddr, addr);

	fprintf(stdout, "%s:\tType: %s  Bus: %s\n", di.name, hci_typetostr((di.type & 0x30) >> 4), hci_bustostr(di.type & 0x0f));
	fprintf(stdout, "\tBD Address: %s  ACL MTU: %d:%d  SCO MTU: %d:%d\n", addr, di.acl_mtu, di.acl_pkts, di.sco_mtu, di.sco_pkts);

	fprintf(stdout, "\n");
	read_class(dev_id, dev_handle);

	fprintf(stdout, "\n");
	read_version(dev_id, dev_handle, di);

	fprintf(stdout, "\n");
	read_afh_mode(dev_id, dev_handle);

	fprintf(stdout, "\n");

	close(dev_handle);

	return 0;
}

int write_name(int dev_id, int dev_handle, char* name)
{
	if (hci_write_local_name(dev_handle, name, 2000) < 0)
	{
		fprintf(stderr, "Can't change local name on hci%d: %s (%d)\n", dev_id, strerror(errno), errno);
		return 1;
	}
	return 0;
}

int write_class(int dev_id, int dev_handle, char* class)
{
	uint32_t code;

	code = (uint32_t) strtoul(class, NULL, 16);
	if (hci_write_class_of_dev(dev_handle, code, 2000) < 0)
	{
		fprintf(stderr, "Can't write local class of device on hci%d: %s (%d)\n", dev_id, strerror(errno), errno);
		return 1;
	}
	return 0;
}

int write_address(int dev_id, int dev_handle, char* address)
{
	return 0;
}

static int read_class(int dev_id, int dev_handle)
{
	static const char* services[] = {"Positioning", "Networking", "Rendering", "Capturing", "Object Transfer", "Audio", "Telephony", "Information"};
	static const char* major_devices[] = {"Miscellaneous", "Computer", "Phone", "LAN Access", "Audio/Video", "Peripheral", "Imaging", "Uncategorized"};

	uint8_t cls[3];
	uint8_t first;
	unsigned int i;

	if (hci_read_class_of_dev(dev_handle, cls, 1000) < 0)
	{
		fprintf(stderr, "Can't read class of device on hci%d: %s (%d)\n", dev_id, strerror(errno), errno);
		return 1;
	}

	fprintf(stdout, "\tDevice class:\t\t0x%02x%02x%02x\n", cls[2], cls[1], cls[0]);
	fprintf(stdout, "\tService classes:\t");
	if (cls[2])
	{
		first = 1;
		for (i = 0; i < (sizeof(services) / sizeof(*services)); i ++)
		{
			if (cls[2] & (1 << i))
			{
				if (! first)
				{
					fprintf(stdout, ", ");
				}
				fprintf(stdout, "%s", services[i]);
				first = 0;
			}
		}
		fprintf(stdout, "\n");
	}
	else
	{
		fprintf(stdout, "Unspecified\n");
	}

	fprintf(stdout, "\tClass description:\t");
	if ((cls[1] & 0x1f) >= sizeof(major_devices) / sizeof(*major_devices))
	{
		fprintf(stdout, "Invalid device class!\n");
	}
	else
	{
		fprintf(stdout, "%s, %s\n", major_devices[cls[1] & 0x1f], get_minor_device_name(cls[1] & 0x1f, cls[0] >> 2));
	}

	return 0;
}

static int read_version(int dev_id, int dev_handle, struct hci_dev_info di)
{
	struct hci_version ver;
	char* hciver, * lmpver;

	if (hci_read_local_version(dev_handle, &ver, 1000) < 0)
	{
		fprintf(stderr, "Can't read version info hci%d: %s (%d)\n", dev_id, strerror(errno), errno);
		return 1;
	}

	hciver = hci_vertostr(ver.hci_ver);
	if (((di.type & 0x30) >> 4) == HCI_BREDR)
	{
		lmpver = lmp_vertostr(ver.lmp_ver);
	}
	else
	{
		lmpver = pal_vertostr(ver.lmp_ver);
	}

	fprintf(stdout, "\tHCI Version: %s (0x%x)  Revision: 0x%x\n"
			"\t%s Version: %s (0x%x)  Subversion: 0x%x\n"
			"\tManufacturer: %s (%d)\n", hciver ? hciver : "n/a", ver.hci_ver, ver.hci_rev, (((di.type & 0x30) >> 4) == HCI_BREDR) ? "LMP" : "PAL", lmpver ? lmpver : "n/a", ver.lmp_ver, ver.lmp_subver, bt_compidtostr(ver.manufacturer), ver.manufacturer);

	if (hciver)
	{
		bt_free(hciver);
	}

	if (lmpver)
	{
		bt_free(lmpver);
	}

	return 0;
}

static int read_afh_mode(int dev_id, int dev_handle)
{
	uint8_t mode;

	if (hci_read_afh_mode(dev_handle, &mode, 1000) < 0)
	{
		fprintf(stderr, "Can't read AFH mode on hci%d: %s (%d)\n", dev_id, strerror(errno), errno);
		return 1;
	}

	fprintf(stdout, "\tAFH mode: %s\n", mode == 1 ? "Enabled" : "Disabled");

	return 0;
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