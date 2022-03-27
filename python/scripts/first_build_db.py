from command import *
import time
import sys
import os

from sqlalchemy import create_engine
from sqlalchemy import MetaData
from sqlalchemy import Table, Column, Integer, String, MetaData, Float, ARRAY

# Create main database
database_name = 'global'

# Check exits
if os.path.exists( '/home/gammapi/DB/' + database_name + '.db' ):
  print("DATABASE Exists", database_name)
  sys.exit()

# Create database if not already there
engine = create_engine('sqlite:////home/gammapi/DB/' + database_name + '.db', echo = True)

# Create Global Config Table first
global_meta = MetaData()
configtable = Table(
  'global_config', global_meta,
  Column('id', String, primary_key = True),
  Column('det0_config', String),
  Column('det1_config', String),
  Column('det2_config', String),
  Column('det3_config', String),
  Column('det0_active', Integer),
  Column('det1_active', Integer),
  Column('det2_active', Integer),
  Column('det3_active', Integer),
  Column('short_time', Integer),
  Column('long_time', Integer),
  Column('run_tag', String),
  Column('data_path', String)
)
global_meta.create_all(engine)

# Update main database definition
f = open("/home/gammapi/.CURRENT_CONFIG","w")
f.write("default")
f.close()

ins = configtable.insert().values(
  id="default",
  det0_config="default_BGO",
  det1_config="default_BGO",
  det2_config="default_NaI",
  det3_config="default_NaI",
  det0_active=1,
  det1_active=1,
  det2_active=0,
  det3_active=0,
  short_time=1,
  long_time=60,
  run_tag="gammascan",
  data_path="hometrial_alpha"
)
conn = engine.connect()
result = conn.execute(ins)

# Now create the tube configuration
tube_meta = MetaData()
tube_table = Table(
  'det_config', tube_meta,
  Column('id', String, primary_key = True),
  Column('lld',Integer),
  Column('hv',Integer),
  Column('in1',Integer),
  Column('gain',Integer),
  Column('pmtgain',Integer),
  Column('out1',Integer),
  Column('out2',Integer),
  Column('correction',Integer),
  Column('p0',Float),
  Column('p1',Float),
  Column('p2',Float),
  Column('p3',Float))

tube_meta.create_all(engine)

ins = tube_table.insert().values(
  id="default_BGO",
  lld=100,
  hv=998,
  in1=1,
  gain=1,
  pmtgain=12,
  out1=1,
  out2=1,
  correction=0,
  p0=0,
  p1=0,
  p2=0,
  p3=0
)

conn = engine.connect()
result = conn.execute(ins)

ins = tube_table.insert().values(
  id="default_NaI",
  lld=100,
  hv=575,
  in1=1,
  gain=1,
  pmtgain=12,
  out1=1,
  out2=1,
  correction=0,
  p0=0,
  p1=0,
  p2=0,
  p3=0
)

conn = engine.connect()
result = conn.execute(ins)




# Create database if not already there
engine = create_engine('sqlite:////home/gammapi/DB/rundata.db', echo = True)

# Create Global Config Table first
global_meta = MetaData()
configtable = Table(
  'gpsdata', global_meta,
  Column('i', Integer, primary_key = True),
  Column('unixtime', Integer),
  Column('run_tag',String),
  Column('timestamp', String),
  Column('datestamp', String),
  Column('latitude', Float),
  Column('longitude', Float),
  Column('altitude', Integer),
  Column('hdop', Float),
  Column('vdop', Float),
  Column('num_sats', Integer),
  Column('age_gps_data', Integer)
)
global_meta.create_all(engine)


configtable = Table(
  'gammadata', global_meta,
  Column('i', Integer, primary_key = True),
  Column('unixtime', Integer),
  Column('run_tag',String),
  Column('short_run_count',Integer),
  Column('long_run_count',Integer),
  Column('detid',Integer),
  Column('hv',Integer),
  Column('lld',Integer),
  Column('exposure',Integer),
  Column('counts',String)
)
global_meta.create_all(engine)


configtable = Table(
  'envirodata', global_meta,
  Column('i', Integer, primary_key = True),
  Column('unixtime', Integer),
  Column('run_tag',String),
  Column('light',Float),
  Column('red',Float),
  Column('green',Float),
  Column('blue',Float),
  Column('acceleration_x',Float),
  Column('acceleration_y',Float),
  Column('acceleration_z',Float),
  Column('magnetometer_x',Float),
  Column('magnetometer_y',Float),
  Column('magnetometer_z',Float),
  Column('heading',Float),
  Column('temperature',Float),
  Column('pressure',Float),
  Column('analog_0',Float),
  Column('analog_1',Float),
  Column('analog_2',Float),
  Column('analog_3',Float)
)
global_meta.create_all(engine)

