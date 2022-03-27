import serial
import pynmea2
from datetime import datetime

from sqlalchemy import create_engine
from sqlalchemy import MetaData
from sqlalchemy import Table, Column, Integer, String, MetaData, Float
from sqlalchemy import select
from sqlalchemy.orm import Session
import sqlalchemy as db
import time
from subprocess import Popen

errors = 0
gps_data = {}
gps_data["gpgga"] = None
gps_data["gprmc"] = None
gps_data["gpgsa"] = None

from db import *


db = dbhandler(["global","rundata"])

print("Global Config")
global_config = db.GetTableEntry("global","global_config","default")

def parseGPS(strv):
    errors = 0
    global gps_data
    try:
        strv = strv.decode()
        print("DEV",strv)
        if ('$GPGGA' in strv): gps_data["gpgga"] = pynmea2.parse(strv)
        if ('$GPRMC' in strv): gps_data["gprmc"] = pynmea2.parse(strv)
        if ('$GPGSA' in strv): gps_data["gpgsa"] = pynmea2.parse(strv)
    except:
        errors += 1 

print("Previous data!")
import pandas as pd
print(pd.DataFrame(data=db.GetTableAll("rundata","gpsdata")))


firsttimestamp = -1

serialPort = serial.Serial("/dev/ttyS0", 115200, timeout=0.5)

cmd="AT+CGPS=0,1;+CGPSAUTO=1;+CGPS=1,1;+CGPSINFOCFG=1,261;+CGPSNMEARATE=1"
serialPort.write(cmd.encode())
print(serialPort.read(128))

lstamp = 0
while True:

    while serialPort.in_waiting:
      strv = serialPort.readline()
      print(strv) #.decode())
      parseGPS(strv)
      time.sleep(0.05)

    if not gps_data["gpgga"]: continue
    if not gps_data["gprmc"]: continue
    if not gps_data["gpgsa"]: continue

    #dataset = ins
    if firsttimestamp <= 0:
      print(gps_data["gprmc"])
      datetimev = datetime.combine(gps_data["gprmc"].datestamp,gps_data["gprmc"].timestamp)
      firsttimestamp = datetime.timestamp(datetimev)
      print("FIRST TIMESTAMP", firsttimestamp, firsttimestamp-time.time())
      clk_id = time.CLOCK_REALTIME
      print("CLK BEFORE", time.time())
      #time.clock_settime(clk_id, firsttimestamp)
      print("CLK AFTER", time.time())
      print("UPDATED TIMESTAMP", firsttimestamp, firsttimestamp-time.time())

    if (str(gps_data["gprmc"].timestamp)) != lstamp:

      global_config = db.GetTableEntry("global","global_config","default")

      ustamp = int(time.time()*1000.0)
      values = { "unixtime": ustamp,
        "run_tag":global_config["run_tag"],
        "timestamp":(str(gps_data["gprmc"].timestamp)),
        "datestamp":(str(gps_data["gprmc"].datestamp)),
        "latitude":gps_data["gpgga"].latitude,
        "longitude":gps_data["gpgga"].longitude,
        "altitude":gps_data["gpgga"].altitude,
        "hdop":gps_data["gpgsa"].hdop,
        "vdop":gps_data["gpgsa"].vdop,
        "num_sats":gps_data["gpgga"].num_sats,
        "age_gps_data":gps_data["gpgga"].age_gps_data
      }

      lstamp = (str(gps_data["gprmc"].timestamp))
      db.AddEntry( "rundata","gpsdata", values )
      print(gps_data["gpgga"].longitude, gps_data["gprmc"].lat, gps_data["gpgga"].latitude, gps_data["gpgsa"].hdop, gps_data["gprmc"].timestamp, gps_data["gprmc"].datestamp, )


