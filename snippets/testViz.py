

import requests
import json


testViz = [
        [-10,0,"wall"],[10,0,"wall"],[-10,10,"wall"],[10,10,"wall"],
        [0,20,"wall"],[-10,20,"wall"],[-10,30,"wall"],[0,30,"wall"],
        [-5,50,"wall"],[0,40,"wall"],[-10,50,"wall"],[-10,30,"wall"],
        [-20,50,"wall"],[-20,30,"wall"],[-30,50,"wall"],[-30,30,"wall"],
        [-40,40,"wall"]];

testMap =[(-10,0,"wall"),

(10,0,"wall"),

(-10,10,"wall"),

(10,10,"wall"),

(0,20,"wall"),

(-10,20,"wall"),

(-10,30,"wall"),

(0,30,"wall"),

(-5,50,"wall"),

(0,40,"wall"),

(-10,50,"wall"),

(-10,30,"wall"),

(-20,50,"wall"),

(-20,30,"wall"),

(-30,50,"wall"),

(-30,30,"wall"),

(-40,40,"wall"), ]

testMap_dict=[]
def getPayLoad(data):
    return {'data':data}
def getXY(data):
    return {'x':data[0],'y':data[1],'tag':data[2]}

for coord in testMap:
    x = getXY(coord)
    b =  json.dumps(x)
    print b
    r = requests.get('http://135.23.47.27:5000/vizFeed',params=getPayLoad(b))
    testMap_dict.append(b)


for one in testMap_dict:
    u = json.loads(one)
    print u['x']


