#!/usr/bin/env python

'''
Author : s-ltf
Date Created : Mar 19.2014
Last Modified by : s-ltf
Last Modified on : Apr 26,2014
'''

#IMPORTS
import Communication_Module.web_server_comm as web_server_comm
import Communication_Module.serial_control as serial_control
import Communication_Module.shared_memory as shared_mem
import Communication_Module.socket_server as socket_server
import Communication_Module.logger as logger
import MMU.mmu as mmu
import sys
from time import sleep
from threading import Thread
#CONSTANTS
SERVER_IP = 'rasp.alatif.ca'
SERVER_PORT = 5000
SERIAL_PORT = r"/dev/ttyACM0"
BAUD_VALUE = 115200
SENSOR_LEN = 9                      #9-bytes long sensor values
RES_SEG_LEN = SENSOR_LEN + 1        #length of rese)
SHARED_MEM_KEY = 44298
SHARED_MEM_SIZE = 200
SOCKET_HOST = '192.168.10.2'
SOCKET_PORT = 5005

MMU_LOG = 'mmu.log'
SOCKET_SERVER_LOG ='socket_server.log'
SERIAL_CONTROL_LOG = 'serial_control.log'
SHARED_MEM_LOG = 'shared_memory.log'
WEB_SERVER_COMM_LOG = 'web_server_comm.log'
CONTROLLER_LOG = 'controller.log'

SWITCHES = ['Verbose','Manual','Socket','Serial','Shared','MMU']
FLAGS ={
    'verbose' : False,
    'manual' : False,
    'socket' : True,
    'mmu' : True,
    'serial' : True,
    'shared' : True,
    }
class processArgs():
    def __init__(self, argv):

        if(len(argv) == 1):
            self.__header()
        else:
            self.__header('CUSTOM')
            for arg in sys.argv[1:]:
                if arg == '-v':
                    self.__set_mode('verbose',True)
                elif arg == '-m':
                    self.__set_mode('Manual',True)
                elif arg == '-socket':
                    self.__set_mode('Socket',True)
                elif arg == '-mmu':
                    self.__set_mode('mmu',False)
                elif arg == '-serial':
                    self.__set_mode('Serial',True)
                elif arg == '-shared':
                    self.__set_mode('Shared',True)
                else:
                    print '\nSwitch Unrecognized'
                    self.__help()
                    exit(0)

        for switch in SWITCHES :
            self.__mode_msg(switch,FLAGS[switch.lower()])

    def __mode_msg(self,name,status):
        if(status):
            status= '\033[92mON\033[0m'
        else:
            status = '\033[91mOFF\033[0m'

        print " %-10s %-3s"%(name,status)

    def __set_mode(self,name,status):
        name = name.lower()
        if name in FLAGS.keys():
            FLAGS[name] = status
        else:
            print 'ERROR switch unrecognized'
            self.__help()

    def __header(self,settings_type='DEFAULT'):
        print "For help use '-h' \n\n"\
        +"RUNNING USING %s VALUES:\n"%(settings_type)\
        +"---------------------------- "

    def __help(self):
        print "...help msg...\n\nmmm...so yeah...use magic!!!\n:P\n"

def read_serial_write_mem(ser,memory,logger):

    [mem_offset, sensor] = ser.read_serial()

    logger.info(str(mem_offset)+'\n')
    #if nothing read from serial, then don't write to memory
    if mem_offset == -1:
        logger.info("bad serial input: %s\n"% sensor)
    else:
        #Add \n to end of sensor value then add leading zeros then add 1 to first char.
        #Total sensor length must equal RES_SEG_LEN
        sensor+='\n'
        sensor = sensor.zfill(RES_SEG_LEN-1)
        sensor = '1' + sensor
        if mem_offset == 2:
            memory.set_mmu_flag_1(True)
        if mem_offset == 3:
            memory.set_mmu_flag_2(True)
        if mem_offset == 4:
            memory.set_mmu_flag_3(True)
        #Write serial input to right place in memory
        memory.write_data_at (sensor, mem_offset)
        logger.info(memory.read_data()+'\n')

def handle_planner_commands(ser,memory,logger):
        '''**********Handle Planner Commands**********'''
        #if flag = 1, read second segment in memory and send planner commands to serial
        #memory.writeData('10+10245n')

        pln_cmd_mem = memory.read_data()

        if  pln_cmd_mem[RES_SEG_LEN*1] == '1':
            logger.info( "Entered planner cmd handling"+'\n')

            #Get second segment from memory and remove leading zeros
            seg = pln_cmd_mem.split('\n')[1]

            #Remove the flag digit and the one leading 0
            seg = seg[2:]
            #Memory format is '10+10275n' Serial format '+10275n'

            logger.header( "***************WRITING TO SERIAL**************"+'\n')
            memory.write_data_at('0', 1)  #Reset planner command flag to 0 to prevent
                                        #reading duplicates
            ser.write_serial(seg)

            #For Debugging , Keep Commented
            #logger.info( "Debug\n" +memory.readData() + "\nend debug\n")
            #exit(0)
        #seg = str(int(seg[1:]))    #This gets rid of the leading zeros
                                    #(ignoring the first digit flag)



def continuous_read(ser,memory,logger):
    while(True):
        read_serial_write_mem(ser,memory,logger)

def main():

    #Process arguments
    processArgs(sys.argv)
    debug = FLAGS['verbose']
    #Initialize the system's various objects
    controller_logger = logger.Logger(CONTROLLER_LOG,debug=debug)
    controller_logger.info('Created Controller logger')

    comm_logger = logger.Logger(WEB_SERVER_COMM_LOG,debug=True )#debug)
    controller_logger.info('Created Web server communication logger')
    comm = web_server_comm.WebServerComm(SERVER_IP,SERVER_PORT,comm_logger)

    shared_mem_logger = logger.Logger(SHARED_MEM_LOG,comm=comm,tag='shared',debug=debug,)
    controller_logger.info('Created Shared Memory logger')
    shared_mem_control = shared_mem.SharedMem(shared_mem_logger,SHARED_MEM_KEY,SHARED_MEM_SIZE)

    serial_logger = logger.Logger(SERIAL_CONTROL_LOG,comm=comm,tag='serial',debug=debug)
    controller_logger.info('Created Serial logger')
    serial = serial_control.SerialControl(SERIAL_PORT,BAUD_VALUE,serial_logger)

    serial_reading_thread = Thread(target=continuous_read,args=(serial,shared_mem_control,controller_logger,))
    serial_reading_thread.daemon = True

    mmu_logger = logger.Logger(MMU_LOG,comm=comm,tag='mmu',debug=debug)
    controller_logger.info('Created MMU logger')
    if FLAGS['mmu']:
        mmu_control = mmu.MMU(shared_mem_control,comm,mmu_logger,serial)
    else:
        # Recovery Flag is on if the mmu flag is given
        mmu_control = mmu.MMU(shared_mem_control,comm,mmu_logger,serial,recovery=True)

    socket_logger = logger.Logger(SOCKET_SERVER_LOG,comm=comm,tag='poi',debug=debug)
    controller_logger.info('Created Socket logger')
    socket_server_control = socket_server.SocketServer(SOCKET_HOST,SOCKET_PORT,comm,socket_logger)

        #Start Modules
    controller_logger.info('Starting Modules')
    raw_input("\nSystem Initilized Press 'ENTER' to run")
    if FLAGS['shared']:
        shared_mem_control.init_mem(SENSOR_LEN,RES_SEG_LEN)
        controller_logger.info('Started Shared Mem Module')
    if FLAGS['serial']:
        serial.start()
        controller_logger.info('Started Serial Module')
    if FLAGS['mmu']:
        mmu_control.start()
        controller_logger.info('Started MMU Module')
    else:
        mmu_control.start()
        controller_logger.info('Started MMU Module With Recovery Flag')
    if FLAGS['socket']:
        socket_server_control.start()
        controller_logger.info('Started Socket Server')


    #Main Controller Logic
    controller_logger.info('Start Main Controller Logic')
    if FLAGS['manual'] :
        serial_reading_thread.start()
        controller_logger.info('Started Serial Port reading thread')
        while True :
            command = raw_input('Enter Value ')
            if(command == 'quit'):
                controller_logger.info('Exiting Manual Control')
                break
            serial.write_serial(command)

    else: #kinda redundant,but for clarity I guess,this branch
          #will run planner control if manual is disabled
        while True:
            read_serial_write_mem(serial,shared_mem_control,controller_logger)

            handle_planner_commands(serial,shared_mem_control,controller_logger)

    try:
        raw_input('press "ENTER" to quit')
        #while True:
        #    sleep(10000)
    except KeyboardInterrupt:
        print '\nStopped by User'
    else:
            
        serial.close()
        shared_mem_control.detach_mem()
        shared_mem_control.remove_mem()





if __name__ == '__main__':
        main()
