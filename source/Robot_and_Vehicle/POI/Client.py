#!/usr/bin/python

import socket
import sys

SERVER = 'localhost'
PORT = 5000

data_to_send = str(sys.argv[1])

#create TCP socket
socket_client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

#connect to server
socket_client.connect((SERVER, PORT))

print 'Data to send: ', str(sys.argv[1])

socket_client.send(data_to_send)
data = socket_client.recv(2048)
socket_client.close()

print 'Receved: ', data
