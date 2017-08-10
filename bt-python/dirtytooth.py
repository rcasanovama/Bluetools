#!/usr/bin/python

import datetime
import sys
import time

import bluetooth
from nOBEX import client, headers, responses

device_address = '00:00:00:00:00:00'


def write_file(filename, data):
    with open(filename, 'w') as f:
        f.write(data)


def get_file(c, src_path, filename, book=True):
    if book:
        mimetype = b'x-bt/phonebook'
    else:
        mimetype = b'x-bt/vcard'

    hdrs, data = c.get(src_path, header_list=[headers.Type(mimetype)])
    write_file(filename, data)
    print('Done!')


def main():
    svc = bluetooth.find_service(address=device_address, uuid='1130')
    if len(svc) < 1:
        sys.stderr.write('Failed to find PBAP service.\n')
        sys.exit(1)

    channel = svc[0]['port']
    c = client.Client(device_address, channel)

    # 796135f0-f0c5-11d8-0966-0800200c9a66
    uuid = b'\x79\x61\x35\xf0\xf0\xc5\x11\xd8\x09\x66\x08\x00\x20\x0c\x9a\x66'
    r = c.connect(header_list=[headers.Target(uuid)])

    if not isinstance(r, responses.ConnectSuccess):
        sys.stderr.write('Failed to connect to phone.\n')
        sys.exit(1)

    date = datetime.datetime.fromtimestamp(time.time()).strftime('%Y%m%d%H%M%S')

    # Phone book
    get_file(c, 'telecom/pb.vcf', '%s_%s-UTC_phonebook' % (device_address, date))

    # Call history
    get_file(c, 'telecom/cch.vcf', '%s_%s-UTC_history' % (device_address, date))

    c.disconnect()


if __name__ == '__main__':
    main()
