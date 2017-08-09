#!/usr/bin/python

import os
import sys

import bluetooth
from PyOBEX import client, headers, responses


def write_file(filename, data):
    f = open(filename, 'w')
    f.write(data)
    f.close()


def get_file(c, src_path, filename, book=True):
    if book:
        mimetype = b'x-bt/phonebook'
    else:
        mimetype = b'x-bt/vcard'

    hdrs, data = c.get(src_path, header_list=[headers.Type(mimetype)])
    write_file(filename, data)
    print('Done!\n')


device_address = '00:00:00:00:00:00'
# device_address = '00:00:00:00:00:00'
prefix = ''  # '/SIM1', 'SIM1', '/'

svc = bluetooth.find_service(address=device_address, uuid='1130')
if len(svc) < 1:
    sys.stderr.write("Failed to find PBAP service.\n")
    sys.exit(1)

channel = svc[0]['port']
c = client.Client(device_address, channel)

# 796135f0-f0c5-11d8-0966-0800200c9a66
uuid = b'\x79\x61\x35\xf0\xf0\xc5\x11\xd8\x09\x66\x08\x00\x20\x0c\x9a\x66'
r = c.connect(header_list=[headers.Target(uuid)])

if not isinstance(r, responses.Success):
    sys.stderr.write("Failed to connect to phone.\n")
    sys.exit(1)

if not os.path.isfile('/home/pi/hacktooth/phonebooks/phonebook_' + device_address):
    get_file(c, prefix + 'telecom/pb.vcf', 'phonebook_' + device_address)

if not os.path.isfile('/home/pi/hacktooth/phonebooks/history_' + device_address):
    get_file(c, prefix + 'telecom/cch.vcf', 'history_' + device_address)

c.disconnect()
