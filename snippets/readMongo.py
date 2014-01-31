from pymongo import MongoClient


'''
Creating a client object to connect to local MongoDB
'''
#client = MongoClient('localhost',27017)
client = MongoClient();

#Connect to ims_test database
db  = client.ims_test

#Collections (similar to tables) not sure if I want to use that
# c = db.ims_test_collection

data = db.logs

#while(True):
for log in data.find():
	print log
