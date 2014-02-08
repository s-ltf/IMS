'''
A class for RPi checking status,will keep track if RPi made a connection to the server or not

Author : s-ltf
Date Created : Feb 03,2014
Last Modified by : s-ltf
Last Modified on : Feb 03,2014
'''

#Imports
from threading import Thread
from time import sleep

#CONSTANTS & Global Variables
N_seconds = 5

#Main Code
class rPi_check():

    def __init__(self):
        '''
        Initiates the tracking process and launches a continuous thread to keep checking status of ip connection
        '''
        self.timedOut = 0
        self.ip = ""
        self.connected = False

        Thread1 = Thread(target = self.timer)
        Thread1.start()


    def timer(self):
        '''
        Contious loop,checks if ip has been set and resets values every N_seconds
        '''
        while (True):
            if(self.timedOut == 0):
                self.ip = "N/A"
                self.connected = False
            else :
                self.timedOut -= 1

            #print self.ip
            sleep(1)
    def setIP (self, ip):
        '''
        sets the IP value and resets the timer

        Keyword arguments:
        ip -- the current ip address of the connected RPi
        '''
        self.ip = ip
        self.connected = True
        self.timedOut = N_seconds

    def getStatus(self):
        '''
        gets the current saved IP value for the RPi
        '''
        old_ip = self.ip
        while (True):
            if(old_ip != self.ip):
                old_ip = self.ip
                if(self.connected):
                    yield "Vehicle is Currently Connected on %s"%(self.ip)
                else:
                    yield "Vehicle is Offline"
