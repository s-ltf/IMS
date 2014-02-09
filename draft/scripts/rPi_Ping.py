#!/usr/bin/env python

'''
Author : s-ltf
Date Created  : Feb 07,2014
Last Modified by : s-ltf
Last Modified on : Feb 07,2014
'''


#Imports
import requests,smtplib,socket
from time import sleep

#CONSTANTS & Global Variables
SERVER_IP = '135.23.47.27'
SERVER_PORT = 5000
FROM_ADDRESS= 'mac_pi@gmail.com' #Not being used atm
TO_ADDRESS  = 'iras146@gmail.com'
HOSTNAME = socket.gethostname()
LOCAL_IP = socket.gethostbyname(HOSTNAME)


#Helper Functions
def generatePayload (ip_address):
    '''
    Takes in an IP address and formats it to be sent using the 
    requsts.get command as parameters

    Keyword arguments:
    ip_address -- the ip address that connects the device to the internet.
    '''
    return {"ip":ip_address}


def getIP():
    '''
    Get the current internet IP address the device is connected through
    '''
    s= socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
    s.connect(('google.com',0))
    public_ip = s.getsockname()[0]

    return public_ip

def compileMessage(public_ip ,hostname = HOSTNAME,local_ip = LOCAL_IP):
    '''
    Compiles a message using the given data to be sent via email

    Keyword arguments:
    public_ip -- The public ip connected the device to the internet
    hostname -- the current hostname of the device
    local_ip -- the set ip address of the device (localhost)
    '''
    msg ="Hello, I'm %s\nI live in %s\nBut hey you can reach me at %s"%(str(hostname),str(local_ip),str(public_ip))

    return msg

def sendEmail( username,password,fromaddr,toaddr,msg):
    '''
    Sends an email with the given credentials to the specified addres containing the provided message

    Keyword arguments:
    username -- user name to be used to log in to the google servers.
    password -- password to be used to log in  to the google servers given the username provided
    fromaddr -- the From address of the email
    toaddr -- the To address of the email
    msg -- the actual content of the email message
    '''
    server = smtplib.SMTP('smtp.gmail.com:587')
    server.starttls()
    server.login(username,password)
    server.sendmail(fromaddr, toaddr, msg)
    server.quit()

    return 0

def ping(public_ip,serverIP = SERVER_IP,serverPort=SERVER_PORT):
    '''
    Will contiously ping the server and provide the currently connected IP address.

    Keyword arguments:
    public_ip -- The public ip connected the device to the internet
    serverIP -- The server ip to connect to
    serverPort -- The port on the server ip to connect to
    '''

    #I may need a while loop here...?
    r = requests.get('http://%s:%s/ping'%(serverIP,serverPort),params=generatePayload(public_ip))

    return 0

#Main Code
def main():

    # Credentials (if needed)
    username = ''
    password = ''

    public_ip = getIP()
    msg = compileMessage(public_ip)

    sendEmail(username,password,FROM_ADDRESS,TO_ADDRESS,msg)

    while(True):
        ping(public_ip)
        sleep(3)

if __name__ == '__main__':

    main()

