#include "../src/bluescan.h"

int main()
{
	int dev_id, dev_handle;
	int i;

	uint8_t num_bluetooth_devices;
	T_BLUETOOTH_DEVICE* bluetooth_devices;

	dev_id = hci_get_route(NULL);
	if (dev_id < 0)
	{
		perror("bluetooth adapter");
		return 1;
	}

	dev_handle = hci_open_dev(dev_id);
	if (dev_handle < 0)
	{
		perror("opening socket");
		return 1;
	}

	num_bluetooth_devices = inquiry_scan(dev_id, dev_handle, &bluetooth_devices);
	for (i = 0; i < num_bluetooth_devices; i ++)
	{
		fprintf(stdout, "%s\t0x%2.2x%2.2x%2.2x\t%-20s\t(clk: 0x%4.4x)\n", bluetooth_devices[i].bdaddr_str, bluetooth_devices[i].dev_class[2], bluetooth_devices[i].dev_class[1], bluetooth_devices[i].dev_class[0], bluetooth_devices[i].name, btohs(bluetooth_devices[i].clock_offset));
	}

	free(bluetooth_devices);
	close(dev_handle);

	return 0;
}