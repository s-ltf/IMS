import os
from threading import Thread
from time import sleep
class myClass():

    def __init__(self):
        self.timedOut = 0
        self.ip = ""
    def timer(self):
        while(True):
            if(self.timedOut == 0):
                self.ip = "NA"
            else:
                self.timedOut -= 1
            print self.ip
            sleep(1)
    def nope(self):
        self.ip = "234"
        self.timedOut = 5
    def getIp(self):
        print self.ip

if __name__ == "__main__":
    Yep = myClass()
    Thread1 = Thread(target = Yep.timer)
    #Thread2 = Thread(target = Yep.getIp)
    Thread1.start()
    #Thread2.start()
    while (True):
        raw_input()
        Yep.nope()
        
