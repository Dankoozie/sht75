import serial
import struct
import sqlite3
import time

SQuery = {
    1: "CREATE TABLE IF NOT EXISTS TempSensor (Sensorid INTEGER, time INTEGER, temp INTEGER, hum INTEGER, setts INTEGER, crc INTEGER)",
    2: "INSERT INTO TempSensor(Sensorid,time,temp,hum,setts,crc) VALUES(?,?,?,?,?,?)"
    }


s = serial.Serial('/dev/ttyUSB0',9600,8,'N',1)

lp = b''


outfile = '/mnt/hdd/www/temp.txt'

#DB stuff
dbc = sqlite3.connect('sensor.db')
dcurr = dbc.cursor()

#Create tables
dcurr.execute(SQuery[1])
dbc.commit()

def savefile(temp,rh):
	f = open(outfile,'w')
	f.write("Current temperature: " + str(temp) + " C\n")
	f.write("Relative humidity: " + str(rh) + "%\n")
	f.write("Last updated: " + time.asctime())
	f.close()



def savereading(sensor,temp,rh,setts,crc):
    dcurr.execute(SQuery[2],(sensor,time.time(),temp,rh,setts,crc))
    dbc.commit()
    


def process_package(pkg):
    sc = struct.unpack(">HhhBBB",pkg)
    if(sc[0] != 65535):
        print("Error packet")
        return 0
    else:
        print("Valid packet received")

    print("Temperature: " + str(sc[1]/100) + "\n")
    print("Humidity: " + str(sc[2]/100) + "\n")
    savereading(1,sc[1]/100,sc[2]/100,0,0)
    savefile(sc[1]/100,sc[2]/100)

def init():
    global lp
    in1 = False
    while(in1 == False):
        b = s.read(1)
        if(b == b'\xff'):
            if(s.read(1) == b'\xff'):
                in1 = True
    lp = b'\xff\xff' 


init()
lp = lp + s.read(7)




while(1):
    process_package(lp)
    lp = s.read(9)
    s.write(b'K')
