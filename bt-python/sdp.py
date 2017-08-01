import bluetooth

# AV_REMOTE_PROFILE_V105 = (bluetooth.AV_REMOTE_CLASS, 0x0105)

# services = bluetooth.find_service()
#
# for i in range(len(services)):
#     name = str(services[i]["name"])
#     port = str(services[i]["port"])
#     host = str(services[i]["host"])
#
#     print('name: ' + name)
#     print('port: ' + port)
#     print('host: ' + host)

server_sock = bluetooth.BluetoothSocket(bluetooth.L2CAP)

port = 0
server_sock.bind(("", 0))
server_sock.listen(1)
print('listening on port %d' % port)

uuid = '1e0ca4ea-299d-4335-93eb-27fcfe7fa848'
bluetooth.advertise_service(server_sock, 'Casanova Service',
                            service_id=uuid,
                            service_classes=[bluetooth.AUDIO_SINK_CLASS],
                            profiles=[bluetooth.AUDIO_SINK_PROFILE])

client_sock, address = server_sock.accept()
print('Accepted connection from ', address)

data = client_sock.recv(1024)
print('received [%s]' % data)

client_sock.close()
server_sock.close()
