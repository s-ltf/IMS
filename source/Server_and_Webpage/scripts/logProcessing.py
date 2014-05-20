'''
A class for Processing new log data that arrives at the server.

Will be responsible for filtering the log stream into the various components of the IMS. and prepare them for the control center webfront

Author : s-ltf
Date Created : Feb 04,2014
Last Modified by : s-ltf
Last Modified on : Feb 04,2014
'''

#Imports
from threading import Thread
from time import sleep
#Constants & Global Variables
N_seconds = 5
IMS_PARTS = ['MMU','SIM','VIZ', 'CCM']

#Main Code
class logProcessing():

    def __init__(self,dbw):
        self.dbw = dbw
        self.allLogs = []
        self.partLogs={}
        for part in IMS_PARTS:
            self.partLogs[part] = []

        #start checking thread


