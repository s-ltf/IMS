#!/usr/bin/python

import socket

HOST = 'localhost'
PORT = 5000

#create a socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

server_socket.bind((HOST, PORT))
server_socket.listen(1)
print 'This server is read to receive...'

while 1:
	conn, addr = server_socket.accept()
	data = conn.recv(2048)
	if data:
		print 'Received: ', data
		conn.send('echo')
	conn.close()

