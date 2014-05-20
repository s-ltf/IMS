'''
Author : s-ltf
Date Created : Feb 13,2014
Last Modified by : s-ltf
Last Modified on : Apr 04,2014
'''

#IMPORTS
import requests
import sys
from time import sleep
#CONSTANTS
TARGETS = {'vizFeed' : 'vizFeed' ,
            'sharedData' : 'sharedData',
            'logData': 'insertLogs',
            'poiFeed' : 'poiFeed',
            'poseData' : 'poseFeed',
            }

SERVER_IP = 'localhost' #'rasp.alatif.ca'
SERVER_PORT = 5000
DELAY = 0
HELP_MSG = 'Usage : python '+sys.argv[0]+' FILE_TO_PARSE'
TRY_COUNTER = 5
def generate_pay_load(data,tag=None,level=None):
        return {'data':data,'tag':tag,'level':level}


def send_data(data,target,tag=None,level=None):
    r = requests.get('http://%s:%s/%s'%
                    (SERVER_IP,SERVER_PORT,TARGETS[target]),params=generate_pay_load(data,tag,level))

    return r

def readFile(name):
    fi = open(name)
    lines = fi.read().split('\n')
    fi.close()
    tag = None
    level = None
    for line in lines:
        try:
            if '[0m' in line:
                try:
                    data,target,tag,level = eval(line[5:-4])
                except:
                    data,target = eval(line[5:-4])
            else:
                try:
                    data,target,tag,level= eval(line)
                except:
                    data,target = eval(line)
            print data,target,tag,level
            r=  send_data(data,target,tag,level)
            sleep(DELAY)
            print r.status_code
        except KeyboardInterrupt:
            break
        except:
            print "line skipped - %s"%line
            pass

def parseArgs():
    global DELAY
    if(len(sys.argv) <= 1):
        print HELP_MSG
        exit(0)
    else:
        file_name = sys.argv[1]
        try:
            DELAY = float(sys.argv[2])
            print 'Delay for %s Seconds'%DELAY
        except:
            DELAY = 0
        return file_name
def main():
    name = parseArgs()
    readFile(name)

if __name__ == '__main__':
    main()
