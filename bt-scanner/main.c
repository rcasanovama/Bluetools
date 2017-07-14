#include "../src/bt-scanner.h"
#include "../src/bt-adapter.h"

int main()
{
	struct btd_adapter* adapter;
	struct btd_device* devices;
	uint8_t number_devices;
	int i;

	adapter = adapter_get_default();

	number_devices = device_inquiry_scan(*adapter, &devices);
	for (i = 0; i < number_devices; i ++)
	{
		device_print_information(devices[i]);
		device_cleanup(&devices[i]);
	}
	free(devices);

	adapter_cleanup(adapter);
	free(adapter);

	return 0;
}