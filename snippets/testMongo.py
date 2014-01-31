from pymongo import MongoClient


'''
Creating a client object to connect to local MongoDB
'''
#client = MongoClient('localhost',27017)

#Connect to ims_test database

#Collections (similar to tables) not sure if I want to use that
# c = db.ims_test_collection


def fake_data(tag,value,test):
	return {'tag' : str(tag) , 'vanue' : value , 'test' : test }

def input( input_data):
	
	client = MongoClient();
#	db  = client.ims_test
#	data = db.logs
	db  = client.ims_cc
	data = db.logs_cc

	return data.insert(input_data)

#for i in range(1000):
#	data_id = data.insert(fake_data('tag_%s'%i,i*2,'test_alpha'))


#for log in data.find():
#	print log
