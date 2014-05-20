'''
Author : s-ltf
Date Created : Feb 13,2014
Last Modified by : s-ltf
Last Modified on : Apr 26,2014
'''

#IMPORTS
import requests
import json
from urllib2 import urlopen,URLError
from threading import Thread
from time import sleep

#CONSTANTS
TARGETS = {'vizFeed' : 'vizFeed' ,
            'sharedData' : 'sharedData',
            'logData': 'insertLogs',
            'poiFeed' : 'poiFeed',
            'poseData' : 'poseFeed',
            }


class WebServerComm():

    def __init__(self,server_ip,server_port,logger):
        self.SERVER_IP= server_ip
        self.SERVER_PORT = server_port
        self.logger = logger
        self.data_buffer = []

        buffer_sending_thread = Thread(target=self.__send_data_buffer)
        buffer_sending_thread.daemon = True
        buffer_sending_thread.start()


    def __check_internet(self):
        try:
            r = urlopen('http://74.125.228.100',timeout=1)
            return True
        except URLError as err:
            pass
        return False

    def __generate_pay_load(self,data,tag=None,level=None):
        return {'data':data,'tag':tag,'level':level}

    def __send_data_buffer(self):
        while(True):
            if(self.data_buffer != []):
                data,target,tag,level = self.data_buffer.pop(0)
                #self.logger.info('Sending Buffered Data: %s'%data)
                try:
                    r = requests.get('http://%s:%s/%s'%
                            (self.SERVER_IP,self.SERVER_PORT,TARGETS[target]),params=self.__generate_pay_load(data,tag,level))
                except:
                    self.data_buffer.insert(0,(data,target,tag,level))
                    #self.logger.error()
                    pass

    def __send_data(self,data,target,tag=None,level=None):
        self.logger.info((data,target,tag,level))
        self.data_buffer.append((data,target,tag,level))
        #if (self.__check_internet()):

            #self.logger.info(r.url+'\n')
        #    if(self.data_buffer != []):
        #        self.__send_data_buffer()
        #else:
            #self.logger.error("No Internet-Buffering Data")
        #except:
        #    pass
            #self.logger.error()
        #return None

    def send_poi_signal(self ,data='door' ):
        return self.__send_data(data,'poiFeed')

    def send_poi_signal2(self ,data='door' ):
        return self.__send_data(data,'poiFeed')


    def send_shared_data(self,data):
        return self.__send_data(data,'sharedData')

    def send_map_data(self, x, y, type_tag):
        map_data = {'x':x,'y':y,'tag':type_tag}
        map_data = json.dumps(map_data)

        r = self.__send_data(map_data,'vizFeed')

        return r 
    def send_pose_data(self,x,y, yaw):
        pose_data = {'x':x, 'y':y, 'yaw':yaw}
        pose_data = json.dumps(pose_data)

        r = self.__send_data(pose_data,'poseData')

        return r 
    def send_log_data(self,data,tag,level='debug'):
        return self.__send_data(data,'logData',tag,level)


