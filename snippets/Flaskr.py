from flask import Flask,request,render_template,Response
import testMongo
import gevent
import testMongoCC
import gevent.monkey
import time
from gevent.pywsgi import WSGIServer
gevent.monkey.patch_all()

app = Flask(__name__)

@app.route('/')
def hello_world():
    return 'Hello World!'

@app.route("/template_test")
def template_test():
    rand_list= [0, 1, 2, 3, 4, 5]
    return render_template('layout.html', a_random_string="Heey, what's up", a_random_list=rand_list)

@app.route('/testLive')
def live_test():
    return render_template('htmlLiveTest.html')
flag = False
def event_stream():
    global flag

    #while True:
    if( flag):
        for i in range(10):
            gevent.sleep(1) 
            yield 'data: %s\n\n'%i
    #    yield 'data: ------------\n\n'
        flag = False
    else:
        yield 'data: test\n\n'          
#        gevent.sleep(5) 
def event_stream2():
    global flag

    while True:
        yield next(testMongoCC.testCapped())
              
#
@app.route('/my_event_source')
#def sse_request():
#    global flag
#    return Response(
#              'data: %s\n\n'%flag ,
#              mimetype='text/event-stream')
#
@app.route('/my_event_source2')
def sse_request2():
    global flag
    return Response(
              event_stream(),
              mimetype='text/event-stream')

@app.route('/my_event_source3')
def sse_request3():
    global flag
    return Response(
              event_stream2(),
              mimetype='text/event-stream')


@app.route('/ping')
def pi_connected():
    
    global flag
    pi_ip = request.args.get('ip','')

    flag = True
    #gevent.sleep(15)
    #flag = False
    return 'data:R-Pi connected on IP %s'% (pi_ip)

@app.route('/testLive2')
def live_test2():
    def g():
          for i,c in enumerate('hello'*10):
              time.sleep(.1)
              yield i,c

    return Response(stream_template('testLive2.html',data=g()))

def stream_template(template_name, **context):
    # http://flask.pocoo.org/docs/patterns/streaming/#streaming-from-templates
    app.update_template_context(context)
    t = app.jinja_env.get_template(template_name)
    rv = t.stream(context)
    # uncomment if you don't need immediate reaction
    ##rv.enable_buffering(5)
    return rv

@app.route('/user/<username>')
def show_user_profile(username):
    # show the user profile for that user
    return 'User %s' % username

@app.route('/post/<int:post_id>')
def show_post(post_id):
    # show the post with the given id, the id is an integer
    return 'Post %d\t%f' % (post_id,second_value)
@app.route('/input')
def test_input():

    response = ''
    tag = request.args.get('tag','')
    value = request.args.get('value','')
    test = request.args.get('test','')
    input_data = testMongo.fake_data(tag,value,test)
    response += 'values to be inputted\n%s\n\n'% input_data
    after_input = testMongo.input(input_data)
    response += 'Obj Id: \t %s'% after_input 
    return response
#     return 'tag : %s\nvalue : %s\ntest :%s'%(tag,value,test)

if __name__ == '__main__':
    app.debug = True
    app.run(host='0.0.0.0',threaded = True)
    #http_server = WSGIServer(('0.0.0.0',5000),app)
    #http_server.serve_forever()
