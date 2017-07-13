#include "../src/bt-adapter.h"

int main()
{
//	struct btd_adapter* adapter = adapter_get_default();
	struct btd_adapter* adapters = NULL;
	uint16_t number_adapters;
	int i;

	number_adapters = adapter_get_all(&adapters);

	for (i = 0; i < number_adapters; i ++)
	{
		adapter_print_information(adapters[i]);
		adapter_cleanup(&adapters[i]);

		printf("\n");
	}
	// show the information of each device
//	hci_for_each_dev(HCI_UP, about_local_device, 0);

//	adapter_cleanup(adapter);
//	adapter_cleanup(adapter);
//	adapter_cleanup(adapter);
//	adapter_cleanup(adapter);

	return 0;
}