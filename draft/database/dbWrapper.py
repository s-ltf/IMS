
'''
A wrapper class for pymongo to work with IMS capstone project scripts and server

Author: s-ltf
Date Created: Feb 01,2014
Last Modified by: s-ltf
Last Modified on: Feb 03,2014

'''

#Import Section
from pymongo import MongoClient

#Global Variables
DEFAULT_DB = 'IMS'
DEFAULT_DB_TEST = 'IMS_TEST'
DEFAULT_PORT = 27017
DEFAULT_IP = 'localhost'
MAX_SIZE = 10000


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


    def queryCC(self,colName,dbName=DEFAULT_DB_TEST):
        """
        Query the set database and collection for any changes,and will keep feeding back
        data 

        Keyword arguments:
        stuff*****

        """
        query={}
        print self.cc[dbName][colName].find()

        cursor = self.cc[dbName][colName].find(query,sort=[('$natural',1)],slave_ok=True,tailable=True,await_data=True)
        while cursor.alive:
            for record in cursor:
                yield record












