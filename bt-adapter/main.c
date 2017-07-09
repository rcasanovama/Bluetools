#include "../src/bt-adapter.h"

int main()
{
	// show the information of each device
	hci_for_each_dev(HCI_UP, about_local_device, 0);

	return 0;
}