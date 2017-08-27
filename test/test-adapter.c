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