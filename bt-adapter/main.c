#include "../src/bt-adapter.h"

int main()
{
	struct btd_adapter* adapters;
	uint16_t number_adapters;
	int i;

	number_adapters = adapter_get_all(&adapters);

	for (i = 0; i < number_adapters; i ++)
	{
		adapter_print_information(adapters[i]);
		adapter_cleanup(&adapters[i]);

		fprintf(stdout, "\n");
	}

	return 0;
}