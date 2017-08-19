#include <stdio.h>

#include "src/pbap-types.h"
#include "src/pbap.h"

void write_to_file(const void* filename, const void* buf, size_t buflen);

int main()
{
	struct pbap_t pbap;

	unsigned char* buf;
	size_t buflen;

	pbap = pbap_client(0, "00:00:00:00:00:00", 13);
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