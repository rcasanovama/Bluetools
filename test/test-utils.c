/**
 *
 *  Copyright (C) 2017  Raul Casanova Marques
 *
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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
