/*
 *
 *  BlueZ - Bluetooth protocol stack for Linux
 *
 *  Copyright (C) 2000-2002  Maxim Krasnyansky <maxk@qualcomm.com>
 *  Copyright (C) 2003-2011  Marcel Holtmann <marcel@holtmann.org>
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <poll.h>
#include <sys/socket.h>

#include "parser/parser.h"

#include "lib/hci.h"
#include "lib/hci_lib.h"

struct hcidump_hdr
{
	uint16_t len;
	uint8_t in;
	uint8_t pad;
	uint32_t ts_sec;
	uint32_t ts_usec;
} __attribute__ ((packed));
#define HCIDUMP_HDR_SIZE (sizeof(struct hcidump_hdr))

struct btsnoop_pkt
{
	uint32_t size;        /* Original Length */
	uint32_t len;        /* Included Length */
	uint32_t flags;        /* Packet Flags */
	uint32_t drops;        /* Cumulative Drops */
	uint64_t ts;        /* Timestamp microseconds */
	uint8_t data[0];    /* Packet Data */
} __attribute__ ((packed));
#define BTSNOOP_PKT_SIZE (sizeof(struct btsnoop_pkt))


static int open_socket(unsigned short hci_dev)
{
	struct sockaddr_hci addr;
	struct hci_filter filter;
	int opt;
	int fd;

	// Create HCI socket
	fd = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
	if (fd < 0)
	{
		perror("Can't create raw socket");
		return 1;
	}

	opt = 1;
	if (setsockopt(fd, SOL_HCI, HCI_DATA_DIR, &opt, sizeof(opt)) < 0)
	{
		perror("Can't enable data direction info");
		close(fd);
		return 2;
	}

	opt = 1;
	if (setsockopt(fd, SOL_HCI, HCI_TIME_STAMP, &opt, sizeof(opt)) < 0)
	{
		perror("Can't enable time stamp");
		close(fd);
		return 3;
	}

	// Setup filter
	hci_filter_clear(&filter);
	hci_filter_all_ptypes(&filter);
	hci_filter_all_events(&filter);
	if (setsockopt(fd, SOL_HCI, HCI_FILTER, &filter, sizeof(filter)) < 0)
	{
		perror("Can't set filter");
		close(fd);
		return 4;
	}

	// Bind socket to the HCI device
	memset(&addr, 0, sizeof(addr));
	addr.hci_family = AF_BLUETOOTH;
	addr.hci_dev = hci_dev;
	if (bind(fd, (struct sockaddr*) &addr, sizeof(addr)) < 0)
	{
		fprintf(stderr, "Can't attach to device hci%d. %s(%d)\n", hci_dev, strerror(errno), errno);
		close(fd);
		return 5;
	}

	return fd;
}

static int process_frames(int fd, uint16_t dev_id, unsigned long flags)
{
	char* data;
	ssize_t len;

	int hdr_size;
	size_t snap_len;

	nfds_t nfds;
	struct pollfd fds[2];

	char ctrl[100];
	struct iovec iv;
	struct frame frm;
	struct msghdr msg;
	struct cmsghdr* cmsg;

	int dir;
	int i, n;

	if (fd < 0)
	{
		return - 1;
	}

	hdr_size = HCIDUMP_HDR_SIZE;
	if (flags & DUMP_BTSNOOP)
	{
		hdr_size = BTSNOOP_PKT_SIZE;
	}
	snap_len = HCI_MAX_FRAME_SIZE;

	data = (char*) malloc(snap_len + hdr_size);
	if (! data)
	{
		perror("Can't allocate data buffer");
		return 1;
	}
	frm.data = data + hdr_size;

//	if (dev_id == HCI_DEV_NONE)
//	{
//		printf("system: ");
//	}
//	else
//	{
//		printf("device: hci%d ", dev_id);
//	}
//	printf("snap_len: %lu filter: 0x%lx\n", snap_len, parser.filter);

	memset(&msg, 0, sizeof(msg));

	nfds = 0;
	fds[nfds].fd = fd;
	fds[nfds].events = POLLIN;
	fds[nfds].revents = 0;
	nfds ++;

	while (1)
	{
		n = poll(fds, nfds, - 1);
		if (n <= 0)
		{
			continue;
		}

		for (i = 0; i < nfds; i ++)
		{
			if (fds[i].revents & (POLLHUP | POLLERR | POLLNVAL))
			{
//				if (fds[i].fd == fd)
//				{
//					printf("device: disconnected\n");
//				}
//				else
//				{
//					printf("client: disconnect\n");
//				}
				return 0;
			}
		}

		iv.iov_base = frm.data;
		iv.iov_len = snap_len;

		msg.msg_iov = &iv;
		msg.msg_iovlen = 1;
		msg.msg_control = ctrl;
		msg.msg_controllen = 100;

		len = recvmsg(fd, &msg, MSG_DONTWAIT);
		if (len < 0)
		{
			if (errno == EAGAIN || errno == EINTR)
			{
				continue;
			}
			perror("Receive failed");
			return - 1;
		}

		// Process control message
		frm.data_len = (uint32_t) len;
		frm.dev_id = dev_id;
		frm.in = 0;
		frm.pppdump_fd = parser.pppdump_fd;
		frm.audio_fd = parser.audio_fd;

		cmsg = CMSG_FIRSTHDR(&msg);
		while (cmsg)
		{
			switch (cmsg->cmsg_type)
			{
				case HCI_CMSG_DIR:
				{
					memcpy(&dir, CMSG_DATA(cmsg), sizeof(int));
					frm.in = (uint8_t) dir;
					break;
				}
				case HCI_CMSG_TSTAMP:
				{
					memcpy(&frm.ts, CMSG_DATA(cmsg), sizeof(struct timeval));
					break;
				}
				default:
				{
					break;
				}
			}
			cmsg = CMSG_NXTHDR(&msg, cmsg);
		}

		frm.ptr = frm.data;
		frm.len = frm.data_len;

		// TODO: implement the mitm here
		parse(&frm);
	}
}

static struct option main_options[] = {
		{ "device",		1, 0, 'i' },
		{ "timestamp",	0, 0, 't' },
		{ "ascii",		0, 0, 'a' },
		{ "hex",		0, 0, 'x' },
		{ "ext",		0, 0, 'X' },
		{ "raw",		0, 0, 'R' },
		{ 0 }
};

int main(int argc, char* argv[])
{
	unsigned long flags = DUMP_VERBOSE;
	unsigned short hci_dev = 0;
	int opt;

	while ((opt = getopt_long(argc, argv, "i:taxXR", main_options, NULL)) != - 1)
	{
		switch (opt)
		{
			case 'i':
			{
				hci_dev = (unsigned short) atoi(optarg + 3);
				break;
			}
			case 't':
			{
				flags |= DUMP_TSTAMP;
				break;
			}
			case 'a':
			{
				flags |= DUMP_ASCII;
				break;
			}
			case 'x':
			{
				flags |= DUMP_HEX;
				break;
			}
			case 'X':
			{
				flags |= DUMP_EXT;
				break;
			}

			case 'R':
			{
				flags |= DUMP_RAW;
				break;
			}
			default:
			{
				break;
			}
		}
	}

	init_parser(flags, (unsigned long) ~ 0L, 0, DEFAULT_COMPID, - 1, - 1);
	process_frames(open_socket(hci_dev), hci_dev, flags);

	return 0;
}
