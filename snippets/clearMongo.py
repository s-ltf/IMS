from pymongo import MongoClient


'''
Creating a client object to connect to local MongoDB
'''
#client = MongoClient('localhost',27017)
client = MongoClient();

#Connect to ims_test database
print client.drop_database('ims_test')
print client.drop_database('lms_cap')
#Collections (similar to tables) not sure if I want to use that
# c = db.ims_test_collection

#data = db.logs


