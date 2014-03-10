'''
Author : s-ltf,saftawey
Date Created : Feb 13.2014
Last Modified by : s-ltf
Last Modified on : Feb 13,2014
'''
#TODO: restructure using a shared mem class

#Imports
import sysv_ipc,requests
from time import sleep
import serial

#CONSTANTS & Global Variables
SERVER_IP = '135.23.47.27'
SERVER_PORT = 5000
SERIAL_PORT = r"/dev/ttyACM0"
BAUD_VALUE = 9600
#Helper Functions
def generatePayload( data):


    return {"data":data}


def fetchData(sharedMemObj):
    memValue = sharedMemObj.read().strip('\0')

    return memValue
def writeData(sharedMemObj, data):
    return sharedMemObj.write('%s'%data)




def sendData(data,serverIP=SERVER_IP,serverPort=SERVER_PORT):
    r = requests.get('http://%s:%s/sharedData'%(serverIP,serverPort),params=generatePayload(data))
    print r

#Main code
def main():
    #TODO Add try - except to handle errors, for now leave it so things would crash
    ser= serial.Serial(SERIAL_PORT,BAUD_VALUE)
    #TODO Check with mohammad about all the "term attribute checks in his C code
    sm = sysv_ipc.SharedMemory(123456,sysv_ipc.IPC_CREAT,size=1024)


    #initilize shared memory
    c = ['0']*100
    for i in range(3,len(c),4):
        c[i]='\n'

    initializeMem = ''.join(c)
    writeData(sm,initializeMem)

    while(True):
        #TODO check if there is a better check if data is avaiable
        if (ser.isOpen()):
            dataFromSer = ser.readline()
            writeData(sm,dataFromSer)
            sendData(dataFromSer

        data =  fetchData(sm)
        sendData(data)
        sleep(1)

if __name__ == '__main__':
    main()
