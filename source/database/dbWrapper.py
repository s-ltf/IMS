
'''
A wrapper class for pymongo to work with IMS capstone project scripts and server

Author: s-ltf
Date Created: Feb 01,2014
Last Modified by: s-ltf
Last Modified on: Feb 03,2014

'''

#Import Section
from pymongo import MongoClient
from threading import Thread
import time
import redis

#Global Variables
DEFAULT_DB = 'IMS'
DEFAULT_DB_TEST = 'IMS_TEST'
DEFAULT_PORT = 27017
DEFAULT_IP = 'localhost'
MAX_SIZE = 10000
IMS_PARTS = ['MMU','SIM','VIZ', 'CCM']


#Helper Functions







#Class Definition

class mongoWrapper():

    def __init__ (self,ip=DEFAULT_IP,port=DEFAULT_PORT,dbName = DEFAULT_DB_TEST):
        """
        Initilizes the mongoWrapper, connects to mongoDB and connects to a database

        Keyword arguments:
        ip -- ip address to where the mongoDB is located
        port -- port number to connect to mongoDB services
        dbName -- database name to connect to (default IMS)
        """
        self.cc = MongoClient(ip,port)
        self.setDB(dbName)
        self.col = None
        self.setCol('logs_cc',capped = True)
        self.allLogs = []
        self.logUpdate = True
        self.partLogs={}
        for part in IMS_PARTS:
            self.partLogs[part] = []
        #Thread1 = Thread(target = self.queryLogs,args=('logs_cc',))
        #Thread1.start()

    def setDB(self,dbName):
        """
        sets the active database

        Keyword arguments:
        dbName -- name of the database to connect to.
        """
        database_names = self.cc.database_names()
        print database_names

        for database in database_names:
            if database.encode("ascii")== dbName:

                print "Database Exist, connecting to {0}".format(dbName)
                self.db = self.cc[dbName]
                return 1

        print "Database Doesn't Exist, creating {0}".format(dbName)
        self.db = self.cc[dbName]
        return 1

    def getDB(self):
        """
        gets the currently connected database name

        """
        if(self.db == None):
            print "DB NOT SET,PLEASE use setDB( databaseName)"
            return 0

        return self.db.name

    def getCurrentColName(self):
        """
        gets the currently selected Collection
        """
        if(self.col == None):
            print "No collections are set atm,try again after choosig a collection, using\n setCol(collectionName)"
            return 0

        return self.col.name

    def setCol(self,colName,capped= False,size = MAX_SIZE):
        """
        sets the active collection

        Keyword arguments:
        colName -- Name of collection to choose
        capped -- using capped collection or regular (default False)
        size -- size of collection,used when collection doesn't exist and we're creating it for the first time (default MAX_SIZE)
        """
        collection_names = self.db.collection_names()
        print collection_names

        for collection in collection_names:
            if collection.encode("ascii")== colName:

                print "Collection Exist, Choosing {0}".format(colName)
                self.col = self.db[colName]
                return 1

        print "Collection Doesn't Exist, creating {0}".format(colName)
        if(capped):
            self.col = self.db.create_collection(colName,capped=capped,size=size)
        else:
            self.col = self.db[colName]
        return 1

    def insert(self,colName , data):
        """
        Inserts data to a specific collection

        Keyword arguments:
        colName -- Name of collection to add data to.
        data -- data to be added to chosen collection
        """
        self.setCol(colName)

        print "Inserting data to Collection {0}\n located in DB {1}".format(self.getCurrentColName(),self.getDB())

        return self.col.insert(data)

    def dropDB(self,dbName):
        """
        Drops a Single database from the server

        Keyword arguments:
        dbName -- Name of database to be dropped
        """
        return self.cc.drop_database(dbName)

    def dropCollection(self,colName):
        """
        Drops a Single collection from a sepcific database

        Keyword arguments:
        dbName -- database to drop the collection from
        colName -- Collection to be dropped
        """

        return self.cc[dbName].drop_collection(colName)

    '''
    def queryCC(self,colName,dbName=DEFAULT_DB_TEST):
        """
        Query the set database and collection for any changes,and will keep feeding back
        data 

        Keyword arguments:
        stuff*****

        """

        lastID = -1
        lastCount = -1
        while(True):
            for log in self.allLogs:
                newID = log['_id']
                print newID
                if(lastID != newID):
                    lastID = newID
                    yield log
    '''

    def queryLogs(self,colName,dbName=DEFAULT_DB_TEST):


        cursor = self.cc[dbName][colName].find(sort=[('$natural',1)],slave_ok=True,tailable=True,await_data=True)

        while cursor.alive:
            try:
                doc = cursor.next()
                print "appending %s"%doc
                self.allLogs.append(doc)
                self.logUpdate =True

            except StopIteration:
                time.sleep(1)



    def queryCC(self,colName,dbName=DEFAULT_DB_TEST):
        """
        Query the set database and collection for any changes,and will keep feeding back
        data 

        Keyword arguments:
        stuff*****

        """


        cursor = self.cc[dbName][colName].find(sort=[('$natural',1)],slave_ok=True,tailable=True,await_data=True)
        print "outside"
        while True:
            if(self.logUpdate):
                self.logUpdate =False
                s = ""
                for log in self.allLogs:
                    s +="data: %s<br>\n"%log
                s+="\n"
                yield s

        '''while(True):
            if(self.logUpdate):
                self.logUpdate = False
                yield self.allLogs[-1]
        '''

class redisWrapper():
    def __init__(self):
        self.red = redis.StrictRedis()
        self.pubsub = self.red.pubsub()



    def subscribe(self,channelName):
        self.pubsub.subscribe(channelName)

        for message in self.pubsub.listen():
            print message
            yield 'data: %s\n\n'%message['data']

    def publish(self,channelName,data):

        self.red.publish(channelName,u'%s'%data)







