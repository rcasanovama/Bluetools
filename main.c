#include <stdint.h>
#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

typedef struct
{
	char bt_address[19];
	char bt_name[HCI_MAX_NAME_LENGTH];
	struct hci_version bt_version;

} T_BLUETOOTH_DEVICE;

int8_t bt_inquiry_scan_devices(T_BLUETOOTH_DEVICE** bt_devices, uint32_t* bt_devices_number);

int main()
{
	uint32_t i;
	T_BLUETOOTH_DEVICE* bt_devices;
	uint32_t bt_devices_number;

	bt_devices = NULL;
	bt_devices_number = 0;

	bt_inquiry_scan_devices(&bt_devices, &bt_devices_number);
	for (i = 0; i < bt_devices_number; i ++)
	{
		fprintf(stdout, "%s\t%s\n", bt_devices[i].bt_address, bt_devices[i].bt_name);
	}

	return 0;
}

int8_t bt_inquiry_scan_devices(T_BLUETOOTH_DEVICE** bt_devices, uint32_t* bt_devices_number)
{
	int32_t i;

	int32_t dev_id, len, num_rsp;
	int64_t flags;
	inquiry_info* ii;

	int32_t dd;

	dev_id = hci_get_route(NULL);
	dd = hci_open_dev(dev_id);
	if (dev_id < 0 || dd < 0)
	{
		return 1;
	}

	len = 8;
	num_rsp = 255;
	flags = IREQ_CACHE_FLUSH;

	ii = (inquiry_info*) malloc(num_rsp * sizeof(inquiry_info));
	assert(ii != NULL);

	num_rsp = hci_inquiry(dev_id, len, num_rsp, NULL, &ii, flags);
	if (num_rsp < 0)
	{
		return 2;
	}

	*bt_devices_number = (uint32_t) num_rsp;
	*bt_devices = (T_BLUETOOTH_DEVICE*) malloc(num_rsp * sizeof(T_BLUETOOTH_DEVICE));
	assert(bt_devices != NULL);

	for (i = 0; i < num_rsp; i ++)
	{
		ba2str(&ii[i].bdaddr, bt_devices[i]->bt_address);
		if (hci_read_remote_name(dd, &ii[i].bdaddr, sizeof(bt_devices[i]->bt_name), bt_devices[i]->bt_name, 0) < 0)
		{
			strncpy(bt_devices[i]->bt_name, "[unknown]", sizeof("[unknown]"));
		}
	}

	close(dd);
	free(ii);

	return 0;
}
