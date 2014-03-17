'''
Author : s-ltf,M.Elsaftawey
Date Created : Feb 13.2014
Last Modified by : s-ltf
Last Modified on : Mar 16,2014

Helpful Refrences:
http://semanchuk.com/philip/sysv_ipc/
http://pyserial.sourceforge.net/pyserial_api.html
'''

#Imports
import sysv_ipc,requests,serial,json
import sys,os
from time import sleep
from sets import Set


#CONSTANTS & Global Variables
SERVER_IP = 'ims.alatif.ca'
SERVER_PORT = 80
SERIAL_PORT = r"/dev/ttyACM0"
BAUD_VALUE = 9600
DEBUG = 0
SENSOR_LEN = 9                      #9-bytes long sensor values
RES_SEG_LEN = SENSOR_LEN + 1        #length of rese)


#Helper Functions
def generatePayload( data):


    return {"data":data}


def fetchData(sharedMemObj):
    memValue = sharedMemObj.read().strip('\0')

    return memValue
def writeData(sharedMemObj, data):
    return sharedMemObj.write('%s'%data)




def sendData(data,serverIP=SERVER_IP,serverPort=SERVER_PORT):
    r = requests.get('http://%s:%s/sharedData'%(serverIP,serverPort),params=generatePayload(data))
    #print r



#Main code
def main():
    print "RUNNING MAIN FUNCTION"
    global DEBUG #To Flip the Global Debug Flag

    #Verbose Flag check
    if len(sys.argv) >= 2 and sys.argv[1] == '-v':
         DEBUG = 1

    #Initilizing Serial,SharedMem Classes
    ser = SerialClass(SENSOR_LEN, RES_SEG_LEN)
    memory = SharedMemClass()
    memory.initMem(SENSOR_LEN, RES_SEG_LEN)

    #For Debugging , Keep Commented
    #memory.writeDataAt("10+10000n\n", 1)
    #ser.writeSerial("+10000n")

    while (True):

        '''**********Handle Serial Input**********'''
        [mem_offset, sensor] = ser.readSerial()

        #if nothing read from serial, then don't write to memory
        if mem_offset == -1:
            if DEBUG == 1:
                print "bad serial input: "+ sensor
        else:
            #Add \n to end of sensor value then add leading zeros then add 1 to first char.
            #Total sensor length must equal RES_SEG_LEN
            sensor+='\n'
            sensor = sensor.zfill(RES_SEG_LEN-1)
            sensor = '1' + sensor

            #Write serial input to right place in memory
            memory.writeDataAt (sensor, mem_offset)
            if DEBUG == 1:
                print memory.readData()

        '''**********Handle Planner Commands**********'''
        #if flag = 1, read second segment in memory and send planner commands to serial
        #memory.writeData('10+10245n')

        pln_cmd_mem = memory.readData()

        if  pln_cmd_mem[RES_SEG_LEN*1] == '1':
            if DEBUG == 1:
                print "Entered planner cmd handling"

            #Get second segment from memory and remove leading zeros
            seg = pln_cmd_mem.split('\n')[1]

            #Remove the flag digit and the one leading 0
            seg = seg[2:]
            #Memory format is '10+10275n' Serial format '+10275n'

            if DEBUG == 1:
                print "***************WRITING TO SERIAL**************"
            memory.writeDataAt('0', 1)  #Reset planner command flag to 0 to prevent
                                        #reading duplicates
            ser.writeSerial(seg)

            #For Debugging , Keep Commented
            #print "Debug\n" +memory.readData() + "\nend debug\n"
            #exit(0)

        #seg = str(int(seg[1:]))    #This gets rid of the leading zeros
                                    #(ignoring the first digit flag)

    ser.close()
    memory.detachMem()
    memory.removeMem()



''' CLASSES '''
#Error Colors
class bcolors():
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'




#Shared Memory Control Class
class SharedMem():

    def __init__(self):
        #Variables/constants
        self.KEY = 65432
        self.MEMORY_SIZE = 200
        self.memory = sysv_ipc
        self.MEMORY_SIZE = int(self.MEMORY_SIZE)

        if DEBUG == 1:
            print"DEBUGGING MODE ENABLED"

        # Create shared memory object -- #(123456,sysv_ipc.IPC_CREAT,size=100)
        self.memory = self.memory.SharedMemory(self.KEY,self.memory.IPC_CREAT|777 ,size=self.MEMORY_SIZE)

    def initMem(self, SENSOR_LEN, RES_SEG_LEN):

        self.SENSOR_LEN = SENSOR_LEN
        self.RES_SEG_LEN = RES_SEG_LEN

        #fill shared memory with integer 0s
        c = ['0']*self.MEMORY_SIZE
        for i in range(self.SENSOR_LEN,len(c),self.RES_SEG_LEN):
            c[i]='\n'

        initializeMem = ''.join(c)            #convert list to clean string
        self.writeData(initializeMem)    #fill all memory with 0s

        # Read value from shared memory
        memory_value = self.memory.read()

        seg = str(self.RES_SEG_LEN).zfill(9) +  '\n'
        self.writeData(seg)

        if DEBUG == 1:
            print "initialize complete: \n"+ memory_value
            print "Initialized Memory Length: %d\n"  %len(memory_value)

    def writeDataAt(self, data, offset):
        return self.memory.write('%s'%data, self.RES_SEG_LEN*offset)

    def writeData(self, data):
        return self.memory.write('%s'%data)

    def readDataAt(self, offset):
        memValue = self.memory.read(offset).strip('\0')
        #see read () function.. could be rewritten better differently
        return memValue

    def readData(self):
        memValue = self.memory.read().strip('\0')
        return memValue

    def removeMem(self):
        self.memory.remove()

    def detachMem(self):
        self.memory.detach()

    def readDataMmu (self):
        data = self.readData()
        stripData = data.split('\n')[2:13]
        print stripData[0][1:]
        return dict([
            ('left', stripData[0][1:]),
            ('right', stripData[1][1:]),
            ('front', stripData[2][1:]),
            ('yaw', stripData[3][1:]),
            ('encod', stripData[4][1:]),
            ('azim', stripData[5][1:]),
            ('zen', stripData[6][1:]),
            ('accelx', stripData[7][1:]),
            ('accely', stripData[8][1:]),
            ('accelz', stripData[9][1:])])

#Serial Port Control
class SerialControl():

    def __init__(self, SENSOR_LEN, RES_SEG_LEN):
        self.SENSOR_LEN = SENSOR_LEN
        self.RES_SEG_LEN = RES_SEG_LEN

        self.ALLOWED_DIGITS = Set('-0123456789.')        #Used for checking valid serial input

        SERIAL_PORT = '/dev/ttyACM0'
        BAUD_VALUE = 115200

        #Give w/r permissions to serial port
        os.chmod("/dev/ttyACM0",0766)

        #Create Serial Port
        try:
            self.ser = serial.Serial(SERIAL_PORT, BAUD_VALUE)
            self.ser.close()
            self.ser.open()
            #time.sleep (3)
        except:
            print bcolors.FAIL + 'ERROR: Failed to Connect Serial Port' + bcolors.ENDC
            exit(0)

    def isNumOnly (self, s):
        if Set(s).issubset(self.ALLOWED_DIGITS):
            return True
        else:
            return False

    def writeSerial(self, data):
        self.ser.write(data)

    def readSerial(self):

        #self.ser.flush()
        #time.sleep (0)

        offset = -1      #This offset will be used when writing sensors to memory..
                         #2 left, 3 right, 4 front, 5 yaw, 6 distance
                         #(0 is for segment length and 1 is for planner commands)
                         #TODO:add more sensors

        raw_data = self.ser.readline()

        if DEBUG == 1:
            print raw_data

        #Quick error check before trying to accessing the raw_data
        if len(raw_data) < 2:
            data = raw_data
            offset = -1
            return [offset, data]

        #remove first 3 chars and last 2 (\n and \0)
        if raw_data[:3] in ('s0_'):
            data = raw_data[raw_data.index('s0_')+3:len(raw_data)-2]
            offset = 2
        elif raw_data[:3] in ('S0_'):
            data = raw_data[raw_data.index('S0_')+3:len(raw_data)-2]
            offset = 2
        elif raw_data[:3] in ('s1_'):
            data = raw_data[raw_data.index('s1_')+3:len(raw_data)-2]
            offset = 3
        elif raw_data[:3] in ('S1_'):
            data = raw_data[raw_data.index('S1_')+3:len(raw_data)-2]
            offset = 3
        elif raw_data[:3] in ('s2_'):
            data = raw_data[raw_data.index('s2_')+3:len(raw_data)-2]
            offset = 4
        elif raw_data[:3] in ('S2_'):
            data = raw_data[raw_data.index('S2_')+3:len(raw_data)-2]
            offset = 4
        elif  raw_data[:2] in ('y_'):
            data = raw_data[raw_data.index('y_')+2:len(raw_data)-2]
            offset = 5
        elif  raw_data[:2] in ('Y_'):
            data = raw_data[raw_data.index('Y_')+2:len(raw_data)-2]
            offset = 5
        elif  raw_data[:2] in ('d_'):
            data = raw_data[raw_data.index('d_')+2:len(raw_data)-2]
            offset = 6
        elif  raw_data[:2] in ('D_'):
            data = raw_data[raw_data.index('D_')+2:len(raw_data)-2]
            offset = 6
        else:
            data = raw_data
            offset = -1

        #print "After trimming: " + data
        #Check if garbage exists in the data, if so ignore it
        #(sometimes serial gives garbage)
        if self.isNumOnly(data) == False:
            data = raw_data
            offset = -1
            if DEBUG == 1:
                print "Serial Error - serial data contains non digit value: " + data

        return [offset, data]



#Server Communication Control
class commModule():
    def __init__(self):
        pass

    def sendMapData(self, x, y, type_tag):
        mapData = {'x':x,'y':y,'tag':type_tag}
        mapData = json.dumps(mapData)

        r = requests.get('http://%s:%s/vizFeed'%
                (SERVER_IP,SERVER_PORT),params=generatePayload(mapData))
        print r
        return r

    def sendPoseData(self,x,y,yaw):
        print "x:%s\ty:%s\tyaw:%s"%(x,y,yaw)


if __name__ == '__main__':
    main()
