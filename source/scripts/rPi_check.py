'''
A class for RPi checking status,will keep track if RPi made a connection to the server or not

Author : s-ltf
Date Created : Feb 03,2014
Last Modified by : s-ltf
Last Modified on : Feb 23,2014
'''

#Imports
from threading import Thread
from time import sleep

#CONSTANTS & Global Variables
IP_TTL = 122
TIME_BETWEEN_PING=20

#Main Code
class rPi_check():

    def __init__(self):
        '''
        Initiates the tracking process and launches a continuous thread to keep checking status of ip connection
        '''
        self.timedOut = 0
        self.ip = "N/A"
        self.connected = False
        self.pingWebfront=True

        Thread1 = Thread(target = self.timer)
        Thread2 = Thread(target = self.pingTimer)
        Thread1.daemon = True
        Thread2.daemon = True
#        Thread1.start()
#        Thread2.start()

    def timer(self):
        '''
        Contious loop,checks if ip has been set and resets values every IP_TTL
        '''
        while (True):
            if(self.timedOut == 0):
                self.ip = "N/A"
                self.connected = False
            else :
                self.timedOut -= 1

            #print self.ip
            sleep(1)
    def pingTimer(self):

        while(True):
            sleep(TIME_BETWEEN_PING)
            self.pingWebfront = True

    def setIP (self, ip):
        '''
        sets the IP value and resets the timer

        Keyword arguments:
        ip -- the current ip address of the connected RPi
        '''
        self.ip = ip
        self.connected = True
        self.timedOut = IP_TTL

    def getStatus(self):
        '''
        gets the current saved IP value for the RPi
        '''
        while (True):
            if(self.pingWebfront):
                self.pingWebfront=False
                if(self.connected and self.ip != 'N/A'):
                    yield "Vehicle is Currently Connected on %s"%(self.ip)
                else:
                    yield "Vehicle is -Offline-"
