'''
a wrapper class for the database that will be used on IMS project

Author :s-ltf
Date Created: Jan 29,2014
Last Modified by: s-ltf
Last Modified : Jan 29,2014

Database being used for our project is mongoDb
'''

#Initial imports
from pymongo import MongoClient


class dbWrapper:
	
	def __init__(this,server_ip='localhost',port=27017,test=False):
		this.server_ip= server_ip
		this.server_port = int(port)
		this.client = MongoClient(this.server_ip,this.server_port)
		this.db = None
		this.Tables= None #in Mongo it's called Collections,but called it Tables for sake of modularity for future changes.


		db = this.connectDb(dbName,test)
		logs = this.connectTable(db,tableName,test)


	def connectDb(this,dbName,test=False):
	
		this.db = this.client.ims_testServer
		return this.db
		
	def connectTable(this,db,tableName,test=False):
		if(test):
			this.Table = db.tableNam
		else:
			this.Table = this.
		return this.Table
