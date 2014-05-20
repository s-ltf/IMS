'''
Author : s-ltf,M.Elsaftawey
Date Created : Feb 13.2014
Last Modified by : s-ltf
Last Modified on : Mar 27,2014

Helpful Refrences:
http://semanchuk.com/philip/sysv_ipc/
'''

#IMPORTS
import sysv_ipc


#Shared Memory Control Class
class SharedMem():

    def __init__(self,logger,key=44298,memory_size=200):
        #Variables/constants
        self.logger=logger
        self.KEY = key
        self.MEMORY_SIZE = memory_size
        self.memory = sysv_ipc
        self.MEMORY_SIZE = int(self.MEMORY_SIZE)
        self.MMU_FLAG_1 = False
        self.MMU_FLAG_2 = False
        self.MMU_FLAG_3 = False

        # Create shared memory object -- #(123456,sysv_ipc.IPC_CREAT,size=100)
        self.memory = self.memory.SharedMemory(self.KEY,self.memory.IPC_CREAT|0666 ,size=self.MEMORY_SIZE)


    def init_mem(self, sensor_len, res_seg_len):

        self.SENSOR_LEN = sensor_len
        self.RES_SEG_LEN = res_seg_len

        #fill shared memory with integer 0s
        c = ['0']*self.MEMORY_SIZE
        for i in range(self.SENSOR_LEN,len(c),self.RES_SEG_LEN):
            c[i]='\n'

        initialize_mem = ''.join(c)            #convert list to clean string
        self.write_data(initialize_mem)    #fill all memory with 0s

        # Read value from shared memory
        memory_value = self.memory.read()

        seg = str(self.RES_SEG_LEN).zfill(9) +  '\n'
        self.write_data(seg)

        self.logger.info( "initialize complete: \n"+ memory_value)
        self.logger.info( "Initialized Memory Length: %d\n"  %len(memory_value))

    def write_data_at(self, data, offset):
        return self.memory.write('%s'%data, self.RES_SEG_LEN*offset)

    def write_data(self, data):
        return self.memory.write('%s'%data)

    def read_data_at(self, offset):
        mem_value = self.memory.read(offset).strip('\0')
        #see read () function.. could be rewritten better differently
        return mem_value

    def read_data(self):
        mem_value = self.memory.read().strip('\0')
        return mem_value

    def remove_mem(self):
        self.memory.remove()

    def detach_mem(self):
        self.memory.detach()

    def set_mmu_flag_1(self,value):
            self.MMU_FLAG_1 = value

    def set_mmu_flag_2(self,value):
            self.MMU_FLAG_2 = value

    def set_mmu_flag_3(self,value):
            self.MMU_FLAG_3 = value

    def read_data_mmu (self):
        while(not self.MMU_FLAG_1 or not self.MMU_FLAG_2 or not self.MMU_FLAG_3):
            pass

        data = self.read_data()
        strip_data = data.split('\n')[2:13]
        self.MMU_FLAG_1 = False
        self.MMU_FLAG_2 = False
        self.MMU_FLAG_3 = False
        #print strip_data[0][1:]
        return dict([
            ('left', strip_data[0][1:]),
            ('right', strip_data[1][1:]),
            ('front', strip_data[2][1:]),
            ('yaw', strip_data[3][1:]),
            ('encod', strip_data[4][1:]),
            ('azim', strip_data[5][1:]),
            ('zen', strip_data[6][1:]),
            ('accelx', strip_data[7][1:]),
            ('accely', strip_data[8][1:]),
            ('accelz', strip_data[9][1:])])


