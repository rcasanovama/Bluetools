/**
 * Author:  Raul Casanova Marques
 * Date:    08/07/17
 */
#include "bt-adapter.h"

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

static int read_class(int dev_id, int dev_handle)
{
	static const char* services[] = {"Positioning", "Networking", "Rendering", "Capturing", "Object Transfer", "Audio", "Telephony", "Information"};
	static const char* major_devices[] = {"Miscellaneous", "Computer", "Phone", "LAN Access", "Audio/Video", "Peripheral", "Imaging", "Uncategorized"};

	uint8_t cls[3];
	int first;
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