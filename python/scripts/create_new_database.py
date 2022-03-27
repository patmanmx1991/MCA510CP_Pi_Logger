from command import *
import time
import sys
import os

from sqlalchemy import create_engine
from sqlalchemy import MetaData
from sqlalchemy import Table, Column, Integer, String, MetaData

# Create main database
database_name = sys.argv[1]

# Check exits
if os.path.exists( '/home/gammapi/DB/' + database_name + '.db' ):
  print("DATABASE Exists", database_name)
  sys.exit()

# Create database if not
engine = create_engine('sqlite:////home/gammapi/DB/' + database_name + '.db', echo = True)

# Update main database definition
f = open("/home/gammapi/.CURRENTDB","w")
f.write(database_name)
f.close()

# Create Global Config Table first
global_meta = MetaData()

configtable = Table(
  'global_config', global_meta,
  

#students = Table(
#   'gamma_survey_test', meta,
#   Column('id', Integer, primary_key = True),
#   Column('tag', String),
#   Column('t0', Integer),
#   Column('t1', Integer),
#   Column('dv', Integer),
#   Column('c0', Integer),
#   Column('c1', Integer),
#   Column('c2', Integer)
#)
#meta.create_all(engine)

#ins = students.insert()
#ins = students.insert().values(tag = 'dbtest', t0 = 1001)
#conn = engine.connect()
#result = conn.execute(ins)







obj = USBGammaSpec()
obj.SetConfig(hv=998, lld=100)

obj2 = USBGammaSpec()
obj2.SetConfig(hv=998, lld=100)

#print("VALID", obj.valid, obj2.valid)

runtag = sys.argv[1]
os.mkdir("data/" + runtag)
i = 0
obj.StartRun()

while True:
    i += 1

    #obj.StartRun()
    time.sleep(1)

    #obj.StopRun()
    
    obj.GetReadings()
    obj2.GetReadings()
    #obj.SaveReadings("data/" + runtag + "/datarun_" + str(i) +".dat")
    for i in range(20,1020):
      print("HIST",i,obj.GetCounts(i), obj2.GetCounts(i))

    #print("NEW RUN")
obj.StopRun()
