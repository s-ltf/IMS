'''
Webserver for the IMS capstone project
will be responsibe for controlling,outputting data and sending data to the webfront page and

Author : s-ltf
Date Created : Feb 02,2014
Last Modified by : s-ltf
Last Modified on : Feb 02,2014
'''

#Imports
from flask import Flask,request,render_template,Response
import gevent
import gevent.monkey
import time
import os
import sys
#sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__),os.pardir, os.pardir)))
print sys.path

import database.dbWrapper as dbw

#Global Variables
PI_IP = ''
SERVER = dbw.mongoWrapper()

#Helper Functions
def formatEventStream(data):
    return "data: %s"%data


def isAlive():
    global PI_IP
    if(PI_IP == ''):
        yield formatEventStream("Not Connected")
    else:
        yield formatEventStream("Connected on IP (%s))"%PI_IP)
        gevent.sleep(2)
        PI_IP = ''

def getLogStream():
    return formatEventStream(SERVER.queryCC('logs_cc'))
    #yield "test"


#Main Code
app = Flask(__name__)

@app.route('/')
def homePage():
    return render_template('homePage.html')

@app.route('/ping')
def pingIP():
    global PI_IP
    PI_IP =  request.args.get('ip')
    return "IP Saved"

@app.route('/isAlive')
def isAlive_sse():
    return Response(
            isAlive(),
            mimetype = 'text/event-stream')

@app.route('/insertLogs')
def insertLogs():

    response = ''

    tag = request.args.get('tag')
    value = request.args.get('value')
    data = requests.args.get('data')

    input_data = formatInputData(tag,value,data)
    print "values to be inputted %s"%input_data
    objID = SERVER.insert('logs',input_data)
    print "Obj ID : %s"%objID

@app.route('/getLogStream')
def getLogStream_sse():
    return Response(
            getLogStream(),
            mimetype = 'text/event-stream')

if __name__ == '__main__':
    app.debug = True
    app.run(host='0.0.0.0',threaded = True)
    #http_server = WSGIServer(('0.0.0.0',5000),app)
    #http_server.serve_forever()
