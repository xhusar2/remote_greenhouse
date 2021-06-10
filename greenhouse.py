import datetime
import time
import random
import urllib.request
import asyncio




class Greenhouse:
    url = "http://192.168.0.200:8000/"
    led = 22
    fan = 24
    pump = 23
    #sensors
    temperature = 20
    humidity =21
    waterLevel=25
    CHECKTIMER = 3 #checks arduino

    # initialize GPIO status variables
    ledSts=0
    fanSts=0
    pumpSts=0
    temperatureSts=20
    humiditySts=40
    waterLevelSts=0
    ledTime=datetime.datetime.now()

    def __init__(self):
        self.ledSts = self.getLedSts()
        self.fanSts = 0
        self.pumpSts = 0
        self.temperatureSts = 20
        self.humiditySts = 40
        self.waterLevelSts = 0
        self.ledTime = datetime.datetime.now()


    def run(self):
        while True:
            self.checkLED()
            self.CheckTemperature()
            time.sleep(self.CHECKTIMER)
        #checkFan();
        #checkPump();


    def checkLED(self):
        time.sleep(3)
        elapsed_time = (datetime.datetime.now()- self.ledTime).total_seconds()
        print(elapsed_time)
        self.getLedSts()
        print(self.ledSts)
        if self.ledSts == 0:
            if elapsed_time > 2:
                self.ledTime = datetime.datetime.now()
                self.setLedSts(1)
                print("Turning led off...")
        else:
            if elapsed_time > (24 - 7):
                self.ledTime = datetime.datetime.now()
                print("Turning led on...")
                self.setLedSts(0)



    def getLedSts(self):
        self.ledSts = int(str(urllib.request.urlopen(self.url+"&V"+str(self.led)+"=?").read(),"UTF-8")[-1])
        return self.ledSts

    def setLedSts(self, value):
        self.ledSts = value
        #print((self.url+"&V"+str(self.led)+"="+str(value)))
        urllib.request.urlopen(self.url+"&V"+str(self.led)+"="+str(value))

    def getFanSts(self):
        print(self.url+"&V"+str(self.fan)+"=?")
        self.fanSts = int(str(urllib.request.urlopen(self.url+"&V"+str(self.fan)+"=?").read(),"UTF-8")[-1])
        return self.fanSts

    def setFanSts(self, value):
        self.fanSts = value
        #print((self.url+"&V"+str(self.led)+"="+str(value)))
        urllib.request.urlopen(self.url+"&V"+str(self.fan)+"="+str(value))

    def getPumpSts(self):
        self.pumpSts = int(str(urllib.request.urlopen(self.url+"&V"+str(self.pump)+"=?").read(),"UTF-8")[-1])
        return self.pumpSts

    def setPumpSts(self, value):
        self.pumpSts = value
        #print((self.url+"&V"+str(self.led)+"="+str(value)))
        urllib.request.urlopen(self.url+"&V"+str(self.pump)+"="+str(value))

    def getTempSts(self):
        #fix -999 error
        print(str(urllib.request.urlopen(self.url+"&V"+str(self.temperature)+"=?").read(),"UTF-8"))
        self.temperatureSts = str(urllib.request.urlopen(self.url+"&V"+str(self.temperature)+"=?").read(),"UTF-8")[-5:]
        return self.temperatureSts
    def getHumidSts(self):
        self.humiditySts = str(urllib.request.urlopen(self.url + "&V" + str(self.humidity) + "=?").read(), "UTF-8")[-5:]
        return self.humiditySts

    def getWaterSts(self):
        self.waterLevelSts = str(urllib.request.urlopen(self.url + "&V" + str(self.waterLevel) + "=?").read(), "UTF-8")[-6:]
        return self.waterLevelSts










