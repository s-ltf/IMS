from sysv_ipc import *
from time import sleep

sm = SharedMemory(123456,IPC_CREAT,size=1024)
i = 0
while(True):
    print sm.write("testSharedMem %s"%i)
    i +=1
    sleep(2)
