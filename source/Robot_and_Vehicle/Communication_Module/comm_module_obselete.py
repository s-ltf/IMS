'''
Author : s-ltf,M.Elsaftawey
Date Created : Feb 13.2014
Last Modified by : s-ltf
Last Modified on : Mar 27,2014

Helpful Refrences:
http://semanchuk.com/philip/sysv_ipc/
http://pyserial.sourceforge.net/pyserial_api.html
'''

#Imports
import requests,json,socket,asyncore
import sys,os
from time import sleep
from sets import Set
from threading import Thread

#CONSTANTS & Global Variables
SERVER_IP = 'sari.alatif.ca'
SERVER_PORT = 5000
SERIAL_PORT = r"/dev/ttyACM0"
BAUD_VALUE = 57600
DEBUG = 0
MANUAL = 0
SOCKET = 0
SENSOR_LEN = 9                      #9-bytes long sensor values
RES_SEG_LEN = SENSOR_LEN + 1        #length of rese)
SOCKET_HOST = '192.168.10.2'
SOCKET_PORT = 5005


def processArgs():
    global DEBUG,MANUAL,SOCKET

    for arg in sys.argv[1:]:
        if arg == '-v':
            DEBUG =1
            print "VERBOSE MODE ON"
        elif arg == '-m':
            MANUAL =1
            print "MANUAL MODE ON"
        elif arg == '-s':
            SOCKET = 1
            print "SOCKET SERVER ON"

#Main code
def main():
    print "RUNNING MAIN FUNCTION"
    processArgs()

   #Initilizing Serial,SharedMem Classes
    ser = SerialControl(SENSOR_LEN, RES_SEG_LEN)
    memory = SharedMem()
    memory.initMem(SENSOR_LEN, RES_SEG_LEN)
    server = SocketServer(SOCKET_HOST,SOCKET_PORT)

    serial_read_thread = Thread(target=continuousRead,args=(ser,memory,))
    serial_read_thread.daemon=True
    if(MANUAL == 1):
        serial_read_thread.start()

    if(SOCKET == 1):
        socket_server_thread =Thread(target=server.start)
        socket_server_thread.daemon=True
        socket_server_thread.start()

    while (True):
        if(MANUAL == 1):
            data = raw_input("Enter Value ")
            ser.ser.write(data)
            continue
        '''**********Handle Serial Input**********'''
        [mem_offset, sensor] = ser.readSerial()

        #if nothing read from serial, then don't write to memory
        if mem_offset == -1:
            if DEBUG == 1:
                logger.write( "bad serial input: "+ sensor)
        else:
            #Add \n to end of sensor value then add leading zeros then add 1 to first char.
            #Total sensor length must equal RES_SEG_LEN
            sensor+='\n'
            sensor = sensor.zfill(RES_SEG_LEN-1)
            sensor = '1' + sensor

            #Write serial input to right place in memory
            memory.writeDataAt (sensor, mem_offset)
            if DEBUG == 1:
                logger.write( memory.readData())


        '''**********Handle Planner Commands**********'''
        #if flag = 1, read second segment in memory and send planner commands to serial
        #memory.writeData('10+10245n')

        pln_cmd_mem = memory.readData()

        if  pln_cmd_mem[RES_SEG_LEN*1] == '1':
            if DEBUG == 1:
                logger.write( "Entered planner cmd handling")

            #Get second segment from memory and remove leading zeros
            seg = pln_cmd_mem.split('\n')[1]

            #Remove the flag digit and the one leading 0
            seg = seg[2:]
            #Memory format is '10+10275n' Serial format '+10275n'

            if DEBUG == 1: 
                logger.write( "***************WRITING TO SERIAL**************")
            memory.writeDataAt('0', 1)  #Reset planner command flag to 0 to prevent
                                        #reading duplicates
            ser.writeSerial(seg)

            #For Debugging , Keep Commented
            #print "Debug\n" +memory.readData() + "\nend debug\n"
            #exit(0)
        #seg = str(int(seg[1:]))    #This gets rid of the leading zeros
                                    #(ignoring the first digit flag)

    ser.close()
    memory.detachMem()
    memory.removeMem()

def continuousRead(ser,memory):
    while(True):
        [mem_offset, sensor] = ser.readSerial()

        #if nothing read from serial, then don't write to memory
        if mem_offset == -1:
            if DEBUG == 1:
                logger.write( "bad serial input: "+ sensor)
        else:
            #Add \n to end of sensor value then add leading zeros then add 1 to first char.
            #Total sensor length must equal RES_SEG_LEN
            sensor+='\n'
            sensor = sensor.zfill(RES_SEG_LEN-1)
            sensor = '1' + sensor

            #Write serial input to right place in memory
            memory.writeDataAt (sensor, mem_offset)
            if DEBUG == 1:
                logger.write( memory.readData())



