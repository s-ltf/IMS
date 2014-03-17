#!/usr/bin/env python


import socket
from time import sleep

HOST = 'localhost'  #we'll use the static IPs here
PORT = 5005

s = socket.socket(socket.AF_INET , socket.SOCK_STREAM)
s.connect((HOST,PORT))
i=0
while True:
    print "Sending 'Test %s'"%i
    s.send("Test %s\n"%i)
    i+=1
    if(i > 10000):
        break
s.close()
