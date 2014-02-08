import requests

payload = {'tag':'sss','value':'78ykgvhjgjvgjhg','data':'test1'}

#r = requests.get('http://135.23.47.27:5000/input',params=payload)
#r = requests.post('http://135.23.47.27:5000/input',params=payload)
#r = requests.patch('http://135.23.47.27:5000/input',params=payload)


for i in xrange(50):
    r = requests.get('http://135.23.47.27:5000/insertLogs',params=payload)

    print "Input URL\t%s\nReturn Status\t%s\nData\t%s\n\n"%(r.url,r.status_code,r.text)
