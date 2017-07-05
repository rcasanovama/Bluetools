#include <stdint.h>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <sys/ioctl.h>

typedef struct
{
	char bt_address[19];
	char bt_name[HCI_MAX_NAME_LENGTH];
	struct hci_version bt_version;

} T_BLUETOOTH_DEVICE;

void query_bt_name(int dev_handle, bdaddr_t* bdaddr);

void query_bt_extra_information(int dev_id, int dev_handle, bdaddr_t* bdaddr);

int main(int argc, char* argv[])
{
	inquiry_info* ii = NULL;
	int max_rsp, num_rsp;
	int dev_id, dev_handle, len, flags;
	int i;

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

	len = 8;
	max_rsp = 255;
	flags = IREQ_CACHE_FLUSH;
	ii = (inquiry_info*) malloc(max_rsp * sizeof(inquiry_info));

	num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
	if (num_rsp < 0)
	{
		perror("hci_inquiry");
	}

	for (i = 0; i < num_rsp; i ++)
	{
		query_bt_name(dev_handle, &(ii + i)->bdaddr);
		query_bt_extra_information(dev_id, dev_handle, &(ii + i)->bdaddr);
	}

	free(ii);
	close(dev_handle);

	return 0;
}


void query_bt_name(int dev_handle, bdaddr_t* bdaddr)
{
	char bt_address[19] = {0};
	char bt_name[HCI_MAX_NAME_LENGTH] = {0};

	ba2str(bdaddr, bt_address);
	if (hci_read_remote_name(dev_handle, bdaddr, sizeof(bt_name), bt_name, 0) < 0)
	{
		strcpy(bt_name, "[unknown]");
	}

	fprintf(stdout, "%s\t%s\n", bt_address, bt_name);
}


void query_bt_extra_information(int dev_id, int dev_handle, bdaddr_t* bdaddr)
{
	uint16_t handle, offset;
	uint8_t features[8], max_page = 0;
	char name[249], * tmp;
	char addr[19] = {0};
	uint8_t mode, afh_map[10];
	struct hci_version version;
	struct hci_dev_info di;
	struct hci_conn_info_req* cr;
	int i, cc = 0;

	if (hci_devinfo(dev_id, &di) < 0)
	{
		perror("Can't get device info");
		exit(1);
	}

	printf("Requesting information ...\n");

	cr = malloc(sizeof(*cr) + sizeof(struct hci_conn_info));
	if (! cr)
	{
		perror("Can't get connection info");
		exit(1);
	}

	bacpy(&cr->bdaddr, bdaddr);
	cr->type = ACL_LINK;
	if (ioctl(dev_handle, HCIGETCONNINFO, (unsigned long) cr) < 0)
	{
		if (hci_create_connection(dev_handle, bdaddr, htobs(di.pkt_type & ACL_PTYPE_MASK), 0, 0x01, &handle, 25000) < 0)
		{
			perror("Can't create connection");
			return;
		}
		sleep(1);
		cc = 1;
	}
	else
	{
		handle = htobs(cr->conn_info->handle);
	}

	ba2str(bdaddr, addr);
	printf("\tBD Address:  %s\n", addr);

	if (hci_read_remote_name(dev_handle, bdaddr, sizeof(name), name, 25000) == 0)
	{
		printf("\tDevice Name: %s\n", name);
	}

	if (hci_read_remote_version(dev_handle, handle, &version, 20000) == 0)
	{
		char* ver = lmp_vertostr(version.lmp_ver);
		printf("\tLMP Version: %s (0x%x) LMP Subversion: 0x%x\n"
					   "\tManufacturer: %s (%d)\n", ver ? ver : "n/a", version.lmp_ver, version.lmp_subver, bt_compidtostr(version.manufacturer), version.manufacturer);
		if (ver)
		{
			bt_free(ver);
		}
	}

	memset(features, 0, sizeof(features));
	hci_read_remote_features(dev_handle, handle, features, 20000);

	if ((di.features[7] & LMP_EXT_FEAT) && (features[7] & LMP_EXT_FEAT))
	{
		hci_read_remote_ext_features(dev_handle, handle, 0, &max_page, features, 20000);
	}

	printf("\tFeatures%s: 0x%2.2x 0x%2.2x 0x%2.2x 0x%2.2x "
				   "0x%2.2x 0x%2.2x 0x%2.2x 0x%2.2x\n", (max_page > 0) ? " page 0" : "", features[0], features[1], features[2], features[3], features[4], features[5], features[6], features[7]);

	tmp = lmp_featurestostr(features, "\t\t", 63);
	printf("%s\n", tmp);
	bt_free(tmp);

	for (i = 1; i <= max_page; i ++)
	{
		if (hci_read_remote_ext_features(dev_handle, handle, i, NULL, features, 20000) < 0)
		{
			continue;
		}

		printf("\tFeatures page %d: 0x%2.2x 0x%2.2x 0x%2.2x 0x%2.2x "
					   "0x%2.2x 0x%2.2x 0x%2.2x 0x%2.2x\n", i, features[0], features[1], features[2], features[3], features[4], features[5], features[6], features[7]);
	}

	if (hci_read_clock_offset(dev_handle, handle, &offset, 1000) < 0)
	{
		perror("Reading clock offset failed");
		exit(1);
	}

	printf("\tClock offset: 0x%4.4x\n", btohs(offset));

	if (hci_read_afh_map(dev_handle, handle, &mode, afh_map, 1000) < 0)
	{
		perror("HCI read AFH map request failed");
	}
	if (mode == 0x01)
	{
		// DGS: Replace with call to btbb_print_afh_map - need a piconet
		printf("\tAFH Map: 0x");
		for (i = 0; i < 10; i ++)
		{
			printf("%02x", afh_map[i]);
		}
		printf("\n");
	}
	else
	{
		printf("AFH disabled.\n");
	}
	free(cr);

	if (cc)
	{
//		usleep(10000);
		hci_disconnect(dev_handle, handle, HCI_OE_USER_ENDED_CONNECTION, 10000);
	}
}