#!/usr/bin/env python

#mostly copied from "http://docs.python.org/release/2.5.2/lib/socket-example.html" , we'll addapt code as needed later on

import socket

HOST = 'localhost'  #we'll use the static IPs here
PORT = 5005

s = socket.socket(socket.AF_INET , socket.SOCK_STREAM)
s.bind((HOST,PORT))
s.listen(1) #how many people are connect to this server, for now we'll leave it as 1 , later we may increase it if other parts switch to socket communications

conn,addr = s.accept()
print 'Connected by %s'%(str(addr))
while True:
    data = conn.recv(1024) #I can't recall if that's in bytes or not,if it is in bytes,is a 1MB sufficient ?
    if not data:
        break
    print data
conn.close()

