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