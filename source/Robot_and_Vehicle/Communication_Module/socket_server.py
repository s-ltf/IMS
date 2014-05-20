'''
Author : s-ltf
Date Created : Mar 30,2014
Last Modified by : s-ltf
Last Modified on : Apr 04,2014
'''

#IMPORTS
import socket
import asyncore
from threading import Thread

class ServerHandler(asyncore.dispatcher_with_send):

    def __init__(self,sock,comm,logger):
        try:
            asyncore.dispatcher_with_send.__init__(self,sock)
            self.logger = logger
            self.comm = comm
        except:
            self.logger.error()
    def handle_read(self):
        try:
            data = self.recv(1024)
            if data:
                self.logger.info( "Recieved %s"%data)
                self.comm.send_poi_signal()
                #Current Distance= self.mmu.get_disp()
                #if currnet - old = x
                #
                #Wait for X amoutn of distance to pass
                self.logger.info( "Sent Door Found Signal")
            else:
                self.logger.warning('NO DATA RECIEVED')
        except:
            self.logger.error()

class SocketServer(asyncore.dispatcher):

    def __init__(self, host, port,comm,logger):
        try:
            asyncore.dispatcher.__init__(self)
            self.logger =logger
            self.comm = comm
            self.create_socket(socket.AF_INET, socket.SOCK_STREAM)
            self.set_reuse_addr()
            self.host = host
            self.port = port

            self.main_thread = Thread(target=self.__run)
            self.main_thread.daemon = True
        except:
            self.logger.error()

    def __run(self):
        self.bind((self.host, self.port))
        self.logger.info( "\nConnecting to %s:%s\n"%(self.host,self.port))
        self.listen(5)
        asyncore.loop()

    def start(self):
        self.main_thread.start()

    def handle_accept(self):
        pair = self.accept()
        if pair is not None:
            self.sock, self.addr = pair
            self.logger.info( 'Incoming connection from %s' % repr(self.addr))
            handler = ServerHandler(self.sock,self.comm,self.logger)

    def handle_close(self):
        self.close()
        self.logger.info( "Closed Connection from %s"% repr(self.addr))

