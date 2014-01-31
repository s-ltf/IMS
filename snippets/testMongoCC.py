from pymongo import MongoClient


'''
Creating a client object to connect to local MongoDB
'''
#client = MongoClient('localhost',27017)

#Connect to ims_test database

#Collections (similar to tables) not sure if I want to use that
# c = db.ims_test_collection


def testCapped():
	client = MongoClient()
	db = client.ims_cc
	#cc = db.create_collection('logs_cc',capped=True,size=10)
	#print cc
	data = db.logs_cc
	#data.insert({'test':'setsets'})
	
	#for log in data.find():
	#	print log
	query = {}

	while True:
		cursor = data.find(query,sort=[('$natural',1)],slave_ok=True,tailable=True,await_data=True)
		while cursor.alive:
			for record in cursor:
			#last= record['_id']
				#print record
				yield 'data: %s\n\n'%record


if __name__ == '__main__':
	print 'huh'
	#testCapped()
#for i in range(1000):
#	data_id = data.insert(fake_data('tag_%s'%i,i*2,'test_alpha'))


