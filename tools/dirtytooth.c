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
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <assert.h>
#include <getopt.h>

#include <bluetooth/bluetooth.h>

#include "../src/pbap-types.h"
#include "../src/pbap.h"

void write_to_file(const void* filename, const void* buf, size_t buflen);

static struct option long_options[] = {
		{"interface", required_argument, 0, 'i'},
		{"address", required_argument, 0, 'a'},
		{"channel", required_argument, 0, 'c'},
		{"help", no_argument, 0, 'h'},
		{0, 0, 0, 0}
};

int main(int argc, char* argv[])
{
	struct pbap_t pbap;

	unsigned char* buf;
	size_t buflen;

	uint16_t dev_id;
	uint8_t channel;
	char* addr;

	int opt;

	/* default values */
	dev_id = 0;
	channel = 0xFF;
	addr = NULL;

	while ((opt = getopt_long(argc, argv, "i:a:c:h", long_options, NULL)) != - 1)
	{
		switch (opt)
		{
			case 'i':
			{
				/* check dev, ex: hci0 */
				assert(strlen(optarg) == 4);
				dev_id = (uint16_t) atoi(optarg + 3);
				break;
			}
			case 'a':
			{
				/* check bluetooth address */
				assert(bachk(optarg) == 0);
				addr = strdup(optarg);

				break;
			}
			case 'c':
			{
				channel = (uint8_t) atoi(optarg);
				break;
			}
			case 'h':
			{
				fprintf(stderr, "Usage: %s [--interface=<hciX>] --address=<00:00:00:00:00:00> --channel=<XX>\n", argv[0]);

				break;
			}
			default:
			{
				break;
			}
		}
	}

	/* check required arguments */
	if (channel == 0xFF || addr == NULL)
	{
		fprintf(stderr, "Usage: %s [--interface=<hciX>] --address=<00:00:00:00:00:00> --channel=<XX>\n", argv[0]);
		return - 1;
	}

	/* start pbap client */
	pbap = pbap_client(dev_id, addr, channel);
	if (pbap.pbap_status == PBAP_INIT_OK)
	{
		/* pull phone book */
		buflen = pbap_get(pbap, TELECOM_PB, sizeof(TELECOM_PB), XBT_PHONEBOOK, sizeof(XBT_PHONEBOOK), (void*) &buf);
		write_to_file("pb.vcf", buf, buflen);

		/* pull call history */
		buflen = pbap_get(pbap, TELECOM_CCH, sizeof(TELECOM_CCH), XBT_PHONEBOOK, sizeof(XBT_PHONEBOOK), (void*) &buf);
		write_to_file("cch.vcf", buf, buflen);

		pbap_cleanup(pbap);
	}

	return 0;
}

void write_to_file(const void* filename, const void* buf, size_t buflen)
{
	FILE* f;
	size_t i;

	f = fopen(filename, "wt");
	if (f != NULL)
	{
		/* write buffer to the file */
		for (i = 0; i < buflen; i ++)
		{
			fprintf(f, "%c", ((unsigned char*) buf)[i]);
		}
		/* close the file */
		fclose(f);
	}
	else
	{
		perror("fopen");
	}
}