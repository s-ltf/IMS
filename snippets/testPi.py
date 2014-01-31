import requests

payload = {'tag':'sss','value':'78ykgvhjgjvgjhg','test':'test1'}

r = requests.get('http://135.23.47.27:5000/input',params=payload)
#r = requests.post('http://135.23.47.27:5000/input',params=payload)
#r = requests.patch('http://135.23.47.27:5000/input',params=payload)
print r.url
print r.status_code
print r.text

