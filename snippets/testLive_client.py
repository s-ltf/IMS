import requests
import sys
import time

payload = {'ip':'11111111111'}

while True:
	try:
		r= requests.get('http://135.23.47.27:5000/ping',params=payload)
		print r.url
    #time.sleep(10)
	except :
		print "Server UnReachable\n%s" % sys.exc_info()[0]
		pass
#r = requests.post('http://135.23.47.27:5000/input',params=payload)
#r = requests.patch('http://135.23.47.27:5000/input',params=payload)
print r.status_code
print r.text

