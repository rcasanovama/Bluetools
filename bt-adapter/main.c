#include "../src/bt-adapter.h"

int main()
{
	int dev_id, dev_handle;

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

	about_local_device(dev_id, dev_handle);

	return 0;
}