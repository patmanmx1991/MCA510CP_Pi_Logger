from flask import Flask
from flask_restful import Resource, Api, reqparse
from command import *
import time
import sys
import os
import serial
import numpy as np
from sqlalchemy import create_engine
from sqlalchemy import MetaData
from sqlalchemy import Table, Column, Integer, String, MetaData, Float
from sqlalchemy import select, update
from sqlalchemy.orm import Session
import sqlalchemy as db
import time
from db import *
import json

app = Flask('crspy')
api = Api(app)
print ("Getting DB")
db = dbhandler(["global","rundata"])

class current_value(Resource):
  def get(self):

   datadict = {}
   print("GAMMA")
   gammadata = (db.GetTableLast("rundata","gammadata"))
   for key in gammadata: 
     print(key,"->",gammadata[key])
     datadict[key] = gammadata[key]

   print("ENVIRO")
   envirodata = (db.GetTableLast("rundata","envirodata"))
   for key in envirodata: 
     print(key,"->",envirodata[key])
     datadict[key] = envirodata[key]

   print("GPS")
   envirodata = (db.GetTableLast("rundata","gpsdata"))
   for key in envirodata: 
     print(key,"->",envirodata[key])
     datadict[key] = envirodata[key]
 
   return datadict, 200

class data_by_selection(Resource):
  def get(self):
    jdata = flreq.get_json()
    datadict = db.GetTableAllByRun("rundata",jdata["table"],jdata["run_tag"])

    return datadict, 200

class enviro_data(Resource):
  def get(self):
    print("ENVIRO")
    envirodata = (db.GetTableAll("rundata","envirodata"))
    return envirodata, 200

class gps_data(Resource):
  def get(self):
    print("gps")
    envirodata = (db.GetTableAll("rundata","gpsdata"))
    return envirodata, 200

class gamma_data(Resource):
  def get(self):
    print("gps")
    envirodata = (db.GetTableAll("rundata","gammadata"))
    return envirodata, 200

from sqlalchemy import update
from flask import request as flreq

class global_config(Resource):
  def get(self):
    datadict = db.GetTableEntry("global","global_config","default")
    return datadict, 200

  def post(self):
    jdata = flreq.get_json()
    print("Response DATA", jdata)
    db.UpdateTableEntry("global","global_config","default",jdata)
    datadict = db.GetTableEntry("global","global_config","default")
    return datadict, 200

api.add_resource(current_value, '/current_value')
api.add_resource(data_by_selection, '/data_by_selection')

api.add_resource(enviro_data, '/enviro_data')
api.add_resource(gps_data, '/gps_data')
api.add_resource(gamma_data, '/gamma_data')
api.add_resource(global_config, '/global_config')

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=4243) # 5000

