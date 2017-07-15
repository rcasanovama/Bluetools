#include <stdio.h>
#include <assert.h>

#include "../src/bt-utils.h"
#include "../src/bt-adapter.h"

int main()
{
	struct btd_adapter_name* adapter_name;
	struct btd_adapter_class* adapter_class;
	struct btd_adapter_address* adapter_address;

	adapter_name = str_to_adapter_name_t("test bluetooth name");
	assert(adapter_name != NULL);
	adapter_print_name(*adapter_name);
	fprintf(stdout, "\n\n");

	adapter_class = str_to_adapter_class_t("104");
	assert(adapter_class != NULL);
	adapter_print_class(*adapter_class);
	fprintf(stdout, "\n");

	adapter_address = str_to_adapter_address_t("00:00:00:00:00:00");
	assert(adapter_address != NULL);
	adapter_print_address(*adapter_address);
	fprintf(stdout, "\n");

	return 0;
}
