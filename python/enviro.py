import serial
import pynmea2

from sqlalchemy import create_engine
from sqlalchemy import MetaData
from sqlalchemy import Table, Column, Integer, String, MetaData, Float
from sqlalchemy import select
from sqlalchemy.orm import Session
import sqlalchemy as db
import time
import pandas as pd
errors = 0
from db import *

db = dbhandler(["global","rundata"])
global_config = db.GetTableEntry("global","global_config","default")


print(pd.DataFrame(data=db.GetTableAll("rundata","envirodata")))

import time
time.sleep(30)

from envirophat import light, motion, weather, leds, analog

lightstate = False

lstamp = 0
while True:
    
    global_config = db.GetTableEntry("global","global_config","default")

    lval = light.light()
    rgbval = light.rgb()
    acc = motion.accelerometer()
    mag = motion.magnetometer()
    heading = motion.heading()
    temp = weather.temperature()
    press = weather.pressure()/100.0
    anal = analog.read_all()

    if lightstate: 
      leds.off()
      lightstate = False
    else:
      leds.on()
      lightstate = True

    ustamp = int(time.time()*1000.0)
    values = { "unixtime": ustamp,
               "run_tag": global_config["run_tag"],
               "light": lval,
               "red": rgbval[0],
               "green": rgbval[1],
               "blue": rgbval[2],
               "acceleration_x": acc[0],
               "acceleration_y": acc[1],
               "acceleration_z": acc[2],
               "magnetometer_x": mag[0],
               "magnetometer_y": mag[1],
               "magnetometer_z": mag[2],
               "heading": heading,
               "temperature": temp,
               "pressure": press,
               "analog_0": anal[0],
               "analog_1": anal[1],
               "analog_2": anal[2],
               "analog_3": anal[3] }

    print(values)
    db.AddEntry( "rundata", "envirodata", values )
    print(values)

    time.sleep(1)

