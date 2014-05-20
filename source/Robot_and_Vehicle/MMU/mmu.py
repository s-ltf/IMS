# This code was written for Python 2.7
'''
Author : Mansour
Date Created : Mar 19.2014
Last Modified by : s-ltf
Last Modified on : Apr 06,2014
'''

import sched
import sys
from time import sleep,time
from threading import Thread,Timer
from math import sin, cos, pi,radians
s = sched.scheduler(time, sleep)

RECOVERY_FILE = './recovery_data.recov'

#TODO NEED MORE TESTING WITH PI!!!! TODO#
# This class provides the updated pose and map 10 times per second
class Update():

    # When timer goes off, send updates
    def __init__(self, sur,logger, daemon=True):
        self.sur = sur
        self.logger = logger
        self.updater_thread = Thread(target = self.__updater_callback)
        self.updater_thread.daemon = daemon
        self.updater_thread.start()

    def __updater_callback(self):
        while True:
            '''send out update 10 times a sec as per spec'''
            sleep(0.1)
            s = self.sur
            data = [s.front(),
                s.right(),
                s.left(),
                s.yaw(),
                s.azim(),
                s.zen(),
                s.accelx(),
                s.accely(),
               s.accelz()]
            #print data

# This class creates a map based on the surroundings class
class SLAM():

    def __init__(self, sur,comm,logger):
        self.sur = sur
        self.comm = comm
        self.logger = logger
        self.__pose_m = pose_t()
        self.__map = []
        self.__sonar_thresh_max = 251
        self.__self_thresh_min = 20
        self.__mapper_update = None
        self.__poser_update = None

        ''' ( self.__mapper, args[, kwargs] )
        create a thread that maps continuosly
        create a thread that updates pose continuosly'''
        self.poser_thread = Thread(target=self.__poser)
        self.poser_thread.daemon = True
        self.poser_thread.start()
        #self.logger.info('Poser Thread started')
        self.mapper_thread = Thread(target=self.__mapper)
        self.mapper_thread.daemon = True
        self.mapper_thread.start()
        #self.logger.info('Mapper Thread started')


    def pose(self):
        '''provides the current pose estimate'''
        return self.__pose_m

    def map_points(self, point_id=None):
        '''provdes all the points that have been detected since
        the point provided.'''
        if point_id == None:
            return self.__map
        else:
            return self.__map[int(point_id)]

    def __mapper(self):
        '''function called by mapping thread
        get data from surroundings
        update map
        rinse and repeat'''
        i = 0
        while True:
            '''The following if else checks that the data has been updated 
                the mapper thread is allowed to map'''
            if self.sur.recent_update() > self.__mapper_update:
                self.__mapper_update = self.sur.recent_update()
            else:
                time.sleep(0.1)
                continue
            try:
                if  self.__self_thresh_min < self.sur.right() < self.__sonar_thresh_max:
                    x = self.pose().x +(self.sur.right() * cos(radians(self.__pose_m.yaw)))
                    y = self.pose().y -(self.sur.right() * sin(radians(self.__pose_m.yaw)))
                    type_tag = 'right'
                    ID = i;
                    mapPoint = map_point_t(x,y,type_tag,ID)
                    #print "mapper: x =%s\ty=%s\t %s"%(x,y,type_tag)
                    '''sending the data should be done in the
                    updates module to make the number of send
                    commands reasonable and synchronized with any
                    additional data that we need to send'''
                    self.logger.info(str(((x,y,type_tag),'vizFeed')))
                    self.comm.send_map_data(x,y,type_tag)
                    self.__map.append(mapPoint)
                    i+=1
                    #print "right detected"

                if self.__self_thresh_min < self.sur.left() < self.__sonar_thresh_max:
                    x = self.pose().x - (self.sur.left() * cos(radians(self.__pose_m.yaw)))
                    y = self.pose().y + (self.sur.left() * sin(radians(self.__pose_m.yaw)))
                    type_tag = 'left'
                    ID = i;
                    mapPoint = map_point_t(x,y,type_tag,ID)
                    self.logger.info(str(((x,y,type_tag),'vizFeed')))
                    self.comm.send_map_data(x,y,type_tag)
                    #self.logger.info( "mapper: x =%s\ty=%s\t %s"%(x,y,type_tag))
                    self.__map.append(mapPoint)
                    i+=1
                    #print "left detected"
            except:
                self.logger.error()

    def __poser(self):
        #updates the pose based on the surrounding class
        #it is called by the pose update thread
        self.__zero_yaw = self.sur.yaw()
        sleep(0.1)
        while True:
            '''The following if else checks that the data has been updated 
                the poser thread is allowed to update the pose of the robot'''
            if self.sur.recent_update() > self.__poser_update:
                self.__poser_update = self.sur.recent_update()
            else:
                time.sleep(0.1)
                continue
            try:
                self.__pose_m.x = self.sur.disp()[0]
                self.__pose_m.y = self.sur.disp()[1]
                self.__pose_m.yaw = self.sur.yaw() - self.__zero_yaw
                self.logger.info(str(((self.__pose_m.x, self.__pose_m.y, self.__pose_m.yaw),'poseData')))
                r = self.comm.send_pose_data(self.__pose_m.x, self.__pose_m.y, self.__pose_m.yaw)
                #self.logger.info('Response %s'%r)
            except:
                self.logger.error()

# This class takes in parsed sensor readings and hides sensor implemetation
# details. Thi way in the future if the snesors change the rest of the
# modules in the MMU do not have to be changed :)
class Surroundings():
    # get te latest string sent from the arduino
    # parse and place the values in their respetive variables
    # Calculate the surroundings based on the sensor readings
    # and abstract away the sensor implementation
    def __init__(self,shared_mem,logger,serial,recovery=False):
        try:
            self.apple = 50
            self.logger = logger
            self.serial= serial #Marc you can use self.serial.write_serial(data) to write to serial
            self.recovery = recovery
            '''internal record of what the displacement'''
            if self.recovery :
                self.serial.write_serial('v')
                self.__displacement = self.__get_recovery_data()
                #self.logger.warning("GETTING RECOVERY DATA")
            else:
                self.__displacement = [0, 0]

            self.__prev_encoder = 0

            '''communication module handle'''
            self.__sim = shared_mem
            self.sensors_thread = Thread(target = self.__get_sensor_data)
            self.sensors_thread.daemon = True
            self.sensors_thread.start()
            #self.logger.info('Sensors Thread started')


            self.__yaw_s    = 0
            self.__front_s  = 0
            self.__right_s  = 0
            self.__left_s   = 0
            self.__encod_s  = 0
            self.__azim_s   = 0
            self.__zen_s    = 0
            self.__accelx_s = 0
            self.__accely_s = 0
            self.__accelz_s = 0
            self.__front_right = 0 #place holder for extra sonars
            self.__front_left = 0 #place holder for extra sonars
            self.__update_stamp = None 
        except:
            self.logger.error()

    def __get_sensor_data(self):
        try:
            while (True):
                __readings = self.__sim.read_data_mmu()
                #self.logger.info(__readings)
                self.__left_s = float(__readings['left'])
                self.__right_s = float(__readings['right'])
                self.__front_s = float(__readings['front'])
                self.__yaw_s = float(__readings['yaw'])
                self.__encod_s = float(float(__readings['encod']))


                '''
                self.__azim_s = float(__readings['azim'])
                self.__zen_s = float(__readings['zen'])
                self.__accelx_s = float(__readings['accelx']);
                self.__accely_s = float(__readings['accely']);
                self.__accelz_s = float(__readings['accelz']);
                '''
                self.__update_stamp = time.time()
                #commented sleep line below because the surroundings module
                #blocks when there is not new data to process
                #sleep(.05)
        except:
            self.logger.error()

    def __save_recovery_data(self):
        file_handler = open(RECOVERY_FILE,'w',0)
        file_handler.write(str(self.__displacement))
        file_handler.close()

    def __get_recovery_data(self):
        try:
            file_handler = open(RECOVERY_FILE)
            line = file_handler.read()
            #self.logger.warning('Recovered %s'%line)
            try:
                disp = eval(line)
                return disp
            except:
                self.logger.error()
                return None
        except:
            #self.logger.info('Failed to get Recovery data,Resetting Values')
            return ([0,0],0)

    def recent_update(self):
        return self.__update_stamp

    def front(self):
        return self.__front_s

    def right(self):
        #if thre is new data
        
        return self.__right_s

    def left(self):
        return self.__left_s

    def yaw(self):
        return self.__yaw_s

    def disp(self):
        self.__displacement = [self.__displacement[0] +
                (self.__encod_s - self.__prev_encoder) *
                sin(radians(self.yaw())),self.__displacement[1] +
                (self.__encod_s - self.__prev_encoder) *
                cos(radians(self.yaw()))]
        #write to temp file self.__displacement and self.yaw()
        self.__save_recovery_data()
        self.__prev_encoder = self.__encod_s
        return self.__displacement

    def azim(self):
        return self.__azim_s

    def zen(self):
        return self.__zen_s

    def accelx(self):
        return self.__accelx_s

    def accely(self):
        return self.__accely_s

    def accelz(self):
        return self.__accelz_s

class map_point_t():
    #this class defines map points
    #(x, y, "string")
    def __init__(self, x, y, type, ID):
        self.x = float(x)
        self.y = float(y)
        self.type = str(type)
        self.ID = int(ID)


class pose_t():
    #this class defines the pose type
    #(x, y, yaw)
    def __init__(self):
        self.x = float(0)
        self.y = float(0)
        self.yaw = float(0)
class MMU():

    def __init__(self,shared_mem,comm,logger,serial,recovery=False):
        self.shared_mem = shared_mem
        self.comm = comm
        self.logger = logger
        self.serial = serial
        self.recovery = recovery

        self.main_thread = Thread(target = self.__run)
        self.main_thread.daemon = True

    def __run(self):

        #self.logger.info('Main Thread Started')
        try:
            self.sur = Surroundings(self.shared_mem,self.logger,self.serial,self.recovery)
            self.up = Update(self.sur,self.logger)
            self.slam = SLAM(self.sur,self.comm,self.logger)
        except:
            self.logger.error()

    def start(self):
        self.main_thread.start() #could take the start of the thread intoa seperate method

