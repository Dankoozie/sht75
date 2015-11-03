import serial
import struct
import sqlite3
import time
import sys
from math import log

SQuery = {
    1: "CREATE TABLE IF NOT EXISTS TempSensor (Sensorid INTEGER, time INTEGER, temp INTEGER, hum INTEGER, setts INTEGER, crc INTEGER)",
    2: "INSERT INTO TempSensor(Sensorid,time,temp,hum,setts,crc) VALUES(?,?,?,?,?,?)"
    }


try: s = serial.Serial('/dev/ttyUSB0',9600,8,'N',1)
except: 
	sys.exit("Could not open serial port")
	


outfile = '/mnt/hdd/www/temp.txt'

#DB stuff
dbc = sqlite3.connect('sensor.db')
dcurr = dbc.cursor()

#Create tables
dcurr.execute(SQuery[1])
dbc.commit()

#Wait for two 0xFF's to arrive on serial port
def getstart():
	res = 0
	while(res != 65535):
		time.sleep(0.5)
		try:
			res = res << 8
			res = res + struct.unpack(">B",s.read(1))[0]
			res = res & 65535
		except:
			print("Error receiving start sequence")
			return False

	return True

#Get the 7 bytes following 0xFFFF
def readsensor():
	pck = b''
	try:
		pck = s.read(7)
	except:
		print("Serial error")
		return False

	return pck



def savefile(temp,rh):
	f = open(outfile,'w')
	f.write("Current temperature: " + str(temp) + " C\n")
	f.write("Relative humidity: " + str(rh) + "%\n")
	f.write("Last updated: " + time.asctime())
	f.close()



def savereading(sensor,temp,rh,setts,crc):
    dcurr.execute(SQuery[2],(sensor,time.time(),temp,rh,setts,crc))
    dbc.commit()
    

def dew(t,rh):
		Dtn = dict(water=243.12, ice=272.62) # Table 9
		Dm = dict(water=17.62, ice=22.46) # Table 9
		'With t and rh provided, does not access the hardware.'
		t_range = 'water' if t >= 0 else 'ice'
		tn, m = Dtn[t_range], Dm[t_range]
		return ( # ch 4.4
			tn * (log(rh / 100.0) + (m * t) / (tn + t))
			/ (m -log(rh / 100.0) - m * t / (tn + t)) )


def process_package(pkg):
    sc = struct.unpack(">hhBBB",pkg)
    print("Temperature: " + str(sc[0]/100))
    print("Humidity: " + str(sc[1]/10) + "\n")
    print("Dew: " + str(dew(sc[0]/100,sc[1]/10)) )
    savereading(1,sc[0]/100,sc[1]/10,0,0)
    savefile(sc[0]/100,sc[1]/100)


while(1):
	if(getstart()):
		res = readsensor()
		process_package(res)
