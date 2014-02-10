#!/usr/bin/env python

'''
Webserver for the IMS capstone project
will be responsibe for controlling,outputting data and sending data to the webfront page and

Author : s-ltf
Date Created : Feb 02,2014
Last Modified by : s-ltf
Last Modified on : Feb 08,2014
'''

#Imports
from flask import Flask,request,render_template,Response,json
import gevent
import gevent.monkey
import time,os,sys
import database.dbWrapper as dbw
import scripts.rPi_check as rpi
import redis

#CONSTANTS & Global Variables
SERVER = dbw.mongoWrapper()
#RED = dbw.redisWrapper()
RED = redis.StrictRedis()
RPI = rpi.rPi_check()
DEFAULT_LOGS = 'logs_cc'
DEFAULT_HOMEPAGE = 'homePage.html'

app = Flask(__name__)

#Helper Functions

def formatEventStream(data):
    '''
    formats the given data as a response string for an event object for javascript call, prefixing the data with "data:"

    Keyword arguments:
    data -- data to be formatted and returned.
    '''

    return "retry:1000\ndata: %s\n\n"%data

def formatInputData(tag,value,data):
    '''
    formats the given data as key-value pairs to be inputted into the database

    Keyword arguments:
    tag --
    value --
    data --
    '''
    return {"tag":tag,"value":value,"data":data}

def isAlive():
    '''
    Queries the RPi's ip address
    '''

    return formatEventStream(RPI.getStatus().next())
    #gevent.sleep(3)

def formatJSON(data):


    response = json.dumps(data)

    return response
def getLogStream():
    '''
    Queries the logs data from the database

    '''
    #return formatEventStream(SERVER.queryCC(DEFAULT_LOGS,dbName='IMS_TEST'))
    pubsub = RED.pubsub()
    pubsub.subscribe('logs')
    for message in pubsub.listen():
        print message
        yield 'data: %s\n\n'%(message['data'])

       #yield RED.subscribe('logs')
    #return(SERVER.queryCC(DEFAULT_LOGS,dbName='IMS_TEST'))



#Main Code

@app.route('/')
def homePage():
    '''
    Returns the default home page
    '''
    return render_template(DEFAULT_HOMEPAGE)

@app.route('/ping')
def pingIP():
    '''
    recieves ping data from RPi,and register the passed IP address
    '''
    pi_ip = request.args.get('ip')
    RPI.setIP(pi_ip)
    return "IP Saved"

@app.route('/isAlive')
def isAlive_sse():
    '''
    Responsible for handling the server side event (SSE) requests regarding status of RPi's connectivity
    '''
    return Response(
            isAlive(),
            mimetype = 'text/event-stream')

@app.route('/insertLogs')
def insertLogs():
    '''
    Recieves log data from RPi and insert them into the database

    '''
    response = ''

    tag = request.args.get('tag')
    value = request.args.get('value')
    data = request.args.get('data')

    input_data = formatInputData(tag,value,data)
    webfront_data = formatJSON(input_data)
    print "values to be inputted %s"%input_data
    print "value sending to webserver %s"%webfront_data
    objID = SERVER.insert(DEFAULT_LOGS,input_data)

    RED.publish('logs','%s'%(webfront_data))
    #RED.publish('logs',input_data)
    return "Obj ID : %s"%objID


@app.route('/getLogStream')
def getLogStream_sse():
    '''
    Responsible for handling the SSE requests regarding the logs saved in the server
    '''
    return Response(
            getLogStream(),
            mimetype = 'text/event-stream')

if __name__ == '__main__':
    app.debug = True
    app.run(host='0.0.0.0',threaded = True)
    #http_server = WSGIServer(('0.0.0.0',5000),app)
    #http_server.serve_forever()
