'''
Author : s-ltf,
Date Created : Apr 04,2014
Last Modified by : s-ltf
Last Modified on : Apr 26,2014

'''

#IMPORTS
import logging
import logging.handlers
import sys

#CONSTANTS
MAX_FILE_SIZE = 536870912 #.5 GB
BACKUP_COUNT = 3 #how many backup files to keep
DEFAULT_LOGS_LOCATION = './Logs/'

SKIP_COUNTER=0
#Error Colors
class bcolors():
    HEADER = '\033[95m'
    INFO = '\033[94m'
    DEBUG = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'


class Logger():

    def __init__(self,log_file_name,log_file_location=DEFAULT_LOGS_LOCATION,comm=None,tag=None,debug=False):

        self.debug = debug
        # Set up a specific logger with our desired output level
        self.logger = logging.getLogger(log_file_name)
        self.logger.setLevel(logging.DEBUG)
        self.comm =comm
        self.tag = tag

        LOG_FILE = log_file_location + log_file_name
        # Add the log message handler to the logger
        handler = logging.handlers.RotatingFileHandler(
                              LOG_FILE, maxBytes=MAX_FILE_SIZE,
                              backupCount=BACKUP_COUNT)

        self.logger.addHandler(handler)
        self.header('-- Initializing Log For %s --'%log_file_name)

    def __send_logs(self,data,level):
        global SKIP_COUNTER
        if self.comm : #and self.debug:
            if SKIP_COUNTER==0:
                self.comm.send_log_data(data,self.tag,level=level)
                SKIP_COUNTER = 10
            else:
                SKIP_COUNTER -= 1
    def header(self,data):
        self.logger.info(bcolors.HEADER + str(data) + bcolors.ENDC)
        self.__send_logs(data,'header')

    def info(self,data):
        if self.debug:
            self.logger.info(bcolors.INFO + str(data) + bcolors.ENDC)
            self.__send_logs(data,'info')
        else:
            pass

    def error(self,data=''):
        try:
            traceback = "\n[line:%s %s]"%(sys.exc_info()[-1].tb_lineno,sys.exc_info()[0:2])
        except:
            traceback ="\n"
        if data != '':
            data += traceback
        else:
            data = traceback
        self.logger.error(bcolors.FAIL + str(data)+ bcolors.ENDC)
        self.__send_logs(data,'error')


    def warning(self,data):
        self.logger.warning(bcolors.WARNING+str(data)+bcolors.ENDC)
        self.__send_logs(data,'warning')

    def debug(self,data):
        self.logger.debug(bcolors.DEBUG + str(data)+bcolors.ENDC)
        self.__send_logs(data,'debug')

