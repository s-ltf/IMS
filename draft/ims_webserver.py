from flask import Flask,request,render_template,Response
#import testMongo
import gevent
#import testMongoCC
import gevent.monkey
import time
from gevent.pywsgi import WSGIServer
gevent.monkey.patch_all()

app = Flask(__name__)

@app.route('/')
def homePage():
    return render_template('homePage.html')
             
if __name__ == '__main__':
    app.debug = True
    app.run(host='0.0.0.0',threaded = True)
    #http_server = WSGIServer(('0.0.0.0',5000),app)
    #http_server.serve_forever()
