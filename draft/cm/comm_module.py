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


#CONSTANTS & Global Variables
SERVER_IP = '135.23.47.27'
SERVER_PORT = 5000

#Helper Functions
def generatePayload( data):


    return {"data":data}


def fetchData(sharedMemObj):
    memValue = sharedMemObj.read().strip('\0')

    return memValue

def sendData(data,serverIP=SERVER_IP,serverPort=SERVER_PORT):
    r = requests.get('http://%s:%s/sharedData'%(serverIP,serverPort),params=generatePayload(data))
    print r

#Main code
def main():
    sm = sysv_ipc.SharedMemory(123456)

    while(True):
        data =  fetchData(sm)
        sendData(data)
        print data
        sleep(1)

if __name__ == '__main__':
    main()
