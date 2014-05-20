#!/usr/bin/env python

'''
Webserver for the IMS capstone project
will be responsibe for controlling,outputting data and sending data to the webfront page and

Author : s-ltf
Date Created : Feb 02,2014
Last Modified by : s-ltf
Last Modified on : May 20,2014
'''

#Imports
from flask import Flask,request,render_template,Response,json
from glob import glob
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
DEFAULT_HOMEPAGE = 'ims_v2.html'
SAVED_MAPS = []
SAVED_MAPS_DIR = 'static/savedMaps'
app = Flask(__name__)

#Helper Functions
#TODO rename all functions to fit the _ naming convention
def formatEventStream(data):
    '''
    formats the given data as a response string for an event object for javascript call, prefixing the data with "data:"

    Keyword arguments:
    data -- data to be formatted and returned.
    '''

    return "retry:1000\ndata: %s\n\n"%data

def formatInputData(tag,level,data):
    '''
    formats the given data as key-value pairs to be inputted into the database

    Keyword arguments:
    tag --
    value --
    data --
    '''
    return {"tag":tag,"level":level,"data":data}

def formatJSON(data):


    response = json.dumps(data)

    return response

#TODO find a different design that doesn't utilize overwriting of global variables
def load_saved_maps():
    global SAVED_MAPS
    SAVED_MAPS=glob(SAVED_MAPS_DIR+'/*')

#Main Code
@app.route('/')
#TODO same way map images are added,add team images.
def homePage():
    '''
    Returns the default home page
    '''
    load_saved_maps()
    return render_template(DEFAULT_HOMEPAGE,images=SAVED_MAPS)

@app.route('/ping')
def pingIP():
    '''
    recieves ping data from RPi,and register the passed IP address
    '''
    pi_ip = request.args.get('ip')
    RPI.setIP(pi_ip)
    return "IP Saved"

def isAlive():
    '''
    Queries the RPi's ip address
    '''
    return formatEventStream(RPI.getStatus().next())

@app.route('/isAlive')
def isAlive_sse():
    '''
    Responsible for handling the server side event (SSE) requests regarding status of RPi's connectivity
    '''
    return Response(
            isAlive(),
            mimetype = 'text/event-stream')

#TODO should be converted to a post method instead of get
@app.route('/insertLogs')
def insertLogs():
    '''
    Recieves log data from RPi and insert them into the database

    '''
    response = ''

    tag = request.args.get('tag')
    level = request.args.get('level')
    data = request.args.get('data')

    input_data = formatInputData(tag,level,data)
    webfront_data = formatJSON(input_data)
    print "values to be inputted %s"%input_data
    print "value sending to webserver %s"%webfront_data
    objID = SERVER.insert(DEFAULT_LOGS,input_data)

    RED.publish('logs','%s'%(webfront_data))
    #RED.publish('logs',input_data)
    return "Obj ID : %s"%objID


#TODO check if instead of 'data:' ,formatEventStream could be used
def getSerialStream():
    '''
    Queries the logs data from the database

    '''
    #return formatEventStream(SERVER.queryCC(DEFAULT_LOGS,dbName='IMS_TEST'))
    pubsub = RED.pubsub()
    pubsub.subscribe('serialData')
    for message in pubsub.listen():
        print message
        yield 'data: %s\n\n'%(message['data'])

@app.route('/getSerialStream')
def getSerialStream_sse():
    '''
    Responsible for handling the SSE requests regarding the logs saved in the server
    '''
    return Response(
            getSerialStream(),
            mimetype = 'text/event-stream')

def getLogStream():
    '''
    Queries the logs data from the database

    '''
    pubsub = RED.pubsub()
    pubsub.subscribe('logs')
    for message in pubsub.listen():
        print message
        yield 'data: %s\n\n'%(message['data'])


@app.route('/getLogStream')
def getLogStream_sse():
    '''
    Responsible for handling the SSE requests regarding the logs saved in the server
    '''
    return Response(
            getLogStream(),
            mimetype = 'text/event-stream')

def getVizFeed():

    pubsub = RED.pubsub()
    pubsub.subscribe("vizFeed")
    for message in pubsub.listen():
        print message
        yield 'data: %s\n\n'%(message['data'])

@app.route('/getVizFeed')
def getVizFeed_sse():

    return Response(
            getVizFeed(),
            mimetype= 'text/event-stream')

def getPoseFeed():

    pubsub = RED.pubsub()
    pubsub.subscribe("poseFeed")
    for message in pubsub.listen():
        print message
        yield 'data: %s\n\n'%(message['data'])

@app.route('/getPoseFeed')
def getPoseFeed_sse():

    return Response(
            getPoseFeed(),
            mimetype= 'text/event-stream')


@app.route('/serialData')
def logSerialData():

    #tag = request.args.get('tag')
    #value = request.args.get('value')
    data = request.args.get('data')

    input_data = formatInputData('','',data)
    webfront_data = formatJSON(input_data)
    print "values to be inputted %s"%input_data
    print "value sending to webserver %s"%webfront_data
    objID = SERVER.insert('serialData',input_data)

    RED.publish('serialData','%s'%(webfront_data))
    return "Logged - Obj ID : %s"%objID


@app.route('/sharedData')
def logSharedMemory():

    data = request.args.get('data')

    input_data = formatInputData('','',data)
    webfront_data = formatJSON(input_data)
    print "values to be inputted %s"%input_data
    print "value sending to webserver %s"%webfront_data
    objID = SERVER.insert('sharedMem',input_data)

    RED.publish('sharedMem','%s'%(webfront_data))
    return "Logged - Obj ID : %s"%objID

last_coord = ''
@app.route('/vizFeed')
def vizData():
    global last_coord
    data = request.args.get('data')
#TODO: add proper extraction of x y coordinates then format them into a proper JSON string.

    input_data = formatInputData('','',data)
    webfront_data = formatJSON(input_data)
    print "values to be inputted %s"%input_data
    print "value sending to webserver %s"%webfront_data
    last_coord = input_data['data']

    objID = SERVER.insert('vizData',input_data)

    RED.publish('vizFeed','%s'%(webfront_data))
    '''
    return "Logged - Obj ID : %s"%objID
    '''
    return "test"

@app.route('/poseFeed')
def poseData():
    data = request.args.get('data')
#TODO: add proper extraction of x y coordinates then format them into a proper JSON string.

    print data

    input_data = formatInputData('','',data)
    webfront_data = formatJSON(input_data)
    print "values to be inputted %s"%input_data
    print "value sending to webserver %s"%webfront_data

    objID = SERVER.insert('poseData',input_data)

    RED.publish('poseFeed','%s'%(webfront_data))
    '''
    return "Logged - Obj ID : %s"%objID
    '''
    return data


@app.route('/poiFeed')
def poiData():
    global last_coord
    data = request.args.get('data')
    print data
    if last_coord != '':
        last_coord=eval(last_coord)
        if(last_coord['tag']== 'left'):
            last_coord['tag'] = 'door'
        else:
            return
        last_coord = unicode(last_coord).replace("'","\"")
        input_data = formatInputData('','',last_coord)
        webfront_data = formatJSON(input_data)
        print "POI values to be inputted %s"%input_data
        print "POI value sending to webserver %s"%webfront_data
        RED.publish('vizFeed','%s'%(webfront_data))

    return data

if __name__ == '__main__':
    app.debug = True
    app.run(host='0.0.0.0',threaded = True)
