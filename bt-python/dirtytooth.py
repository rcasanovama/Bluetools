#!/usr/bin/python

import bluetooth
import PyOBEX

devices = bluetooth.discover_devices()

for device in devices:
    print(device)