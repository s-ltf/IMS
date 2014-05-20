'''
Author : s-ltf,M.Elsaftawey
Date Created : Feb 13.2014
Last Modified by : s-ltf
Last Modified on : Apr 03,2014

Helpful Refrences:
http://pyserial.sourceforge.net/pyserial_api.html
'''

#IMPORTS
import serial
import sys
import os
from time import sleep
from sets import Set

#Serial Port Control
class SerialControl():

    def __init__(self, SERIAL_PORT,BAUD_VALUE,logger):
        self.logger = logger
        self.ALLOWED_DIGITS = Set('-0123456789.')        #Used for checking valid serial input


        #Give w/r permissions to serial port
        try:
            os.chmod(SERIAL_PORT,0766)
        except:
            self.logger.error()
        #Create Serial Port
        try:
            self.ser = serial.Serial(SERIAL_PORT, BAUD_VALUE)
            self.start()
        except:
            self.logger.error( 'ERROR: Failed to Create Serial Port')

    def start(self):
        try:
            self.ser.close()
            self.ser.open()
            #sleep (3)
            self.ser.flushInput()
            self.ser.flushOutput()
        except:
            self.logger.error('ERROR: Failed to Connect Serial Port')


    def __is_num_only (self, s):
        if Set(s).issubset(self.ALLOWED_DIGITS):
            return True
        else:
            return False

    def write_serial(self, data):
        self.logger.info('Writing to Serial %s'%data)
        self.ser.write(data)

    def read_serial(self):


        offset = -1      #This offset will be used when writing sensors to memory..
                         #2 left, 3 right, 4 front, 5 yaw, 6 distance
                         #(0 is for segment length and 1 is for planner commands)
                         #TODO:add more sensors

        raw_data = self.ser.readline()

        self.logger.info('Read from Serial: %s'%raw_data)

        #Quick error check before trying to accessing the raw_data
        if len(raw_data) < 2:
            data = raw_data
            offset = -1
            return [offset, data]

        #remove first 3 chars and last 2 (\n and \0)
        if raw_data[:3] in ('s0_'):
            data = raw_data[raw_data.index('s0_')+3:len(raw_data)-2]
            offset = 4  #Replaced Left with Front sensor memory index
        elif raw_data[:3] in ('S0_'):
            data = raw_data[raw_data.index('S0_')+3:len(raw_data)-2]
            offset = 4  #Replaced Left with Front sensor memory index
        elif raw_data[:3] in ('s1_'):
            data = raw_data[raw_data.index('s1_')+3:len(raw_data)-2]
            offset = 3
        elif raw_data[:3] in ('S1_'):
            data = raw_data[raw_data.index('S1_')+3:len(raw_data)-2]
            offset = 3
        elif raw_data[:3] in ('s2_'):
            data = raw_data[raw_data.index('s2_')+3:len(raw_data)-2]
            offset = 2  #Replaced Front with Left sensor memory index
        elif raw_data[:3] in ('S2_'):
            data = raw_data[raw_data.index('S2_')+3:len(raw_data)-2]
            offset = 2  #Replaced Front with Left sensor memory index
        elif  raw_data[:2] in ('y_'):
            data = raw_data[raw_data.index('y_')+2:len(raw_data)-2]
            offset = 5
        elif  raw_data[:2] in ('Y_'):
            data = raw_data[raw_data.index('Y_')+2:len(raw_data)-2]
            offset = 5
        elif  raw_data[:2] in ('d_'):
            data = raw_data[raw_data.index('d_')+2:len(raw_data)-2]
            offset = 6
        elif  raw_data[:2] in ('D_'):
            data = raw_data[raw_data.index('D_')+2:len(raw_data)-2]
            offset = 6
        else:
            data = raw_data
            offset = -1

        #print "After trimming: " + data
        #Check if garbage exists in the data, if so ignore it
        #(sometimes serial gives garbage)
        if self.__is_num_only(data) == False:
            data = raw_data
            offset = -1
            self.logger.error( "Serial Error - serial data contains non digit value: %s"%data)

        return [offset, data]


