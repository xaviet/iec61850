# /usr/bin/python3
# -*- coding: utf-8 -*-
# scdanalysis.py
# by pioevh@163.com 20170411
# g_    global value
# c_    class value
# v_    parameter value
# m_    member value
# t_    temporary value

'''
  xmlanalysis [xmlZipFile]
  
'''

import pdb
# pdb.set_trace()
import logging
logging.basicConfig(level=logging.DEBUG)
# logging.debug('This is debug message')
# logging.info('This is info message')
# logging.warning('This is warning message')
import time
import zipfile
import sys
import xml.etree.ElementTree as elementTree
import sqlite3

g_zipFile='./fengqiao220.zip'
g_xmlExtendName='scd'

def spentTime(v_fun):
  '''
    decorator spentTime
  '''
  def t_decoratorfun(*v_para):
    t_starttime=time.time()
    t_rt=v_fun(*v_para)
    logging.debug('spentTime: '+'%5.6f'%((time.time()-t_starttime),)+' s.')
    return(t_rt)
  return(t_decoratorfun)

class c_xmlDataParser():
  m_atttrib={'maxDepth':0,'numIed':0,'numDataSet':0,'numAddress':0}
  m_xmlData={'attrib':m_atttrib}
  m_depth=0
  m_path=[]
  m_tag=''
  m_data=''
  m_currentParser=''
  m_recoder={}
  m_gse=[]
  m_smv=[]
  m_dataSet=[]
  m_tagParser={}
  
  def parseGoose(self,v_mode):
    if(v_mode==0):
      self.m_recoder['cbName']=self.m_path[-1][1]['cbName']
      self.m_recoder['ldInst']=self.m_path[-1][1]['ldInst']
      self.m_recoder['iedName']=self.m_path[-2][1]['iedName']
      self.m_recoder['apName']=self.m_path[-2][1]['apName']
    elif(v_mode==1):
      if(self.m_data!=''):
        if(self.m_path[-1][0]=='P'):
          self.m_recoder[self.m_path[-1][1]['type']]=self.m_data
        else:
          self.m_recoder[self.m_path[-1][0]]=self.m_data
    elif(v_mode==2):
      self.m_gse.append(self.m_recoder)
      self.m_recoder={}
  
  def parseSmv(self,v_mode):
    if(v_mode==0):
      self.m_recoder['cbName']=self.m_path[-1][1]['cbName']
      self.m_recoder['ldInst']=self.m_path[-1][1]['ldInst']
      self.m_recoder['iedName']=self.m_path[-2][1]['iedName']
      self.m_recoder['apName']=self.m_path[-2][1]['apName']
    elif(v_mode==1):
      if(self.m_data!=''):
        self.m_recoder[self.m_path[-1][1]['type']]=self.m_data
    elif(v_mode==2):
      self.m_smv.append(self.m_recoder)
      self.m_recoder={}

  def parseDataSet(self,v_mode):
    if(v_mode==0):
      print(self.m_path)
    elif(v_mode==1):
      if(self.m_data!=''):
        print(self.m_data)
    elif(v_mode==2):
      self.m_dataSet.append(self.m_recoder)
      self.m_recoder={}
      
  def registerTagParser(self):
    self.m_tagParser['GSE']=self.parseGoose
    self.m_tagParser['SMV']=self.parseSmv
    self.m_tagParser['DataSet']=self.parseDataSet
  
  def __init__(self):
    self.registerTagParser()
  
  def popNamespace(self,v_tag):
    t_tag=v_tag.split('}')[-1]
    return(t_tag)
  
  def start(self,v_tag,v_attrib): 
    self.m_tag=self.popNamespace(v_tag)
    self.m_path.append((self.m_tag,v_attrib))
    self.m_depth+=1
    if(self.m_depth>self.m_xmlData['attrib']['maxDepth']):
      self.m_xmlData['attrib']['maxDepth']=self.m_depth
    if(self.m_tag=='IED'):
      self.m_xmlData['attrib']['numIed']+=1
    if(self.m_tag=='DataSet'):
      self.m_xmlData['attrib']['numDataSet']+=1
    if(self.m_tag=='Address'):
      self.m_xmlData['attrib']['numAddress']+=1 
    if(self.m_tag in self.m_tagParser):
      self.m_currentParser=self.m_tag
      self.m_tagParser[self.m_tag](0)
        
  def end(self,v_tag):
    t_tag=self.popNamespace(v_tag)
    if(t_tag in self.m_tagParser):
      self.m_tagParser[self.m_path[-1][0]](2) 
      self.m_currentParser=''
    self.m_path=self.m_path[:-1]        
    self.m_depth-=1
    
  def data(self,v_data):
    self.m_data=v_data.strip()
    if(self.m_currentParser!=''):
      self.m_tagParser[self.m_currentParser](1)
    
  def close(self):   
    return((self.m_xmlData,self.m_gse,self.m_smv))
        
class c_xmlFile():

  def __init__(self,v_xmlFile):
    pass

class c_zipFile():
  
  def __init__(self,v_zipFile,v_xmlFileExt):
    self.m_zipFile=zipfile.ZipFile(v_zipFile,'r')
    self.m_xmlFileExt=v_xmlFileExt
    
  def __enter__(self):
    t_xmlData=''
    for el0 in self.m_zipFile.namelist():
      logging.debug(' zip list: %s'%(el0,))
      if(el0.endswith((self.m_xmlFileExt))):
        t_xmlData=self.m_zipFile.read(el0) 
        break
    return(t_xmlData)
    
  def __exit__(self,exc_ty,exc_val,tb):
    self.m_zipFile.close()

@spentTime
def main(v_zipFile,v_xmlExtendName):
  t_zipFile=c_zipFile(v_zipFile,v_xmlExtendName)
  with t_zipFile as t_xmlData:
    #logging.debug(' xml root: %s'%(elementTree.fromstring(t_xmlData),))
    t_xmlTarget=c_xmlDataParser()
    #elementTree.register_namespace('','http://www.iec.ch/61850/2003/SCL')
    #elementTree.register_namespace('xsi','http://www.w3.org/2001/XMLSchema-instance')
    #elementTree.register_namespace('sf','http://www.sifang.com"')
    t_xmlParser=elementTree.XMLParser(target=t_xmlTarget)
    t_xmlParser.feed(t_xmlData)
    t_xmlData=t_xmlParser.close()
    print(t_xmlData[1][0],'\n',t_xmlData[2][0])
  
if(__name__=='__main__'):
  g_zipFile=sys.argv[1] if(len(sys.argv)>1) else g_zipFile
  logging.debug(' %s'%(g_zipFile,))
  main(g_zipFile,g_xmlExtendName)

'''
SCL
Header
History
Hitem
Substation
Private
VoltageLevel
Voltage
Bay
SF_IED
Communication
SubNetwork
ConnectedAP
GSE
Address
P
MinTime
MaxTime
PhysConn
SMV
IED
Services
DynAssociation
GetDirectory
GetDataObjectDefinition
DataObjectDirectory
GetDataSetValue
DataSetDirectory
ConfDataSet
DynDataSet
ReadWrite
ConfReportControl
GetCBValues
ReportSettings
GOOSE
FileHandling
ConfLNs
LogSettings
ConfLogControl
AccessPoint
Server
Authentication
LDevice
LN0
DataSet
FCDA
ReportControl
TrgOps
OptFields
RptEnabled
LogControl
DOI
DAI
Val
SDI
LN
SettingControl
Inputs
ExtRef
GSEControl
SettingGroups
SGEdit
ConfSG
TimerActivatedControl
SampledValueControl
SmvOpts
SMVSettings
SmpRate
DataTypeTemplates
LNodeType
DO
DOType
DA
SDO
DAType
BDA
EnumType
EnumVal
'''
