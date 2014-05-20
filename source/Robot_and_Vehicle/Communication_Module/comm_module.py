'''
Author : s-ltf
Date Created : Feb 13,2014
Last Modified by : s-ltf
Last Modified on : Apr 04,2014

'''

#IMPORTS
import requests
import json
from time import sleep
from threading import Thread

#CONSTANTS
TARGETS = {'vizFeed' : 'vizFeed' ,
            'sharedData' : 'sharedData',
            'logData': 'logData',
            'poiFeed' : 'poiFeed',
            }


class CommModule():

    def __init__(self,server_ip,server_port,logger,debug=0):
        self.SERVER_IP= server_ip
        self.SERVER_PORT = server_port
        self.logger = logger
        self.debug = debug

    def __generate_pay_load(data):
        return {'data':data}

    def __send_data(self,data,target):
        r = requests.get('http://%s:%s/%s'%
                (self.SERVER_IP,self.SERVER_PORT,TARGETS[target]),params=generatePayload(data))
        if (self.debug == 1):
            self.logger.info(r.url +'\n')


    def send_poi_signal(self ,data='door' ):
        __send_data(data,'poiFeed')

    def send_shared_data(self,data):
        __send_data(data,'sharedData')


