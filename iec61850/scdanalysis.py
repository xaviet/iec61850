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
import os

g_zipFile='.{0}fengqiao220.zip'.format(os.sep)
g_xmlExtendName='scd'
g_dbPath='.{0}db{0}fengqiao.sdb'.format(os.sep)
g_gooseTab='goose'
g_smvTab='smv'
g_vtTab='vt'
g_vtMatch=['prefix','lnClass','lnInst','doName','daName','ldInst']

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

def sqliteOpt(v_db,v_opt):
  t_rtArray=[]
  t_dbCon=sqlite3.connect(v_db)
  t_dbCur=t_dbCon.cursor()
  t_dbCur.execute(v_opt)
  t_rtArray=t_dbCur.fetchall()
  t_dbCon.commit()
  t_dbCur.close()
  t_dbCon.close()
  return(t_rtArray)

def sqliteOptBatch(v_db,v_opt):
  
  t_dbCon=sqlite3.connect(v_db)
  t_dbCur=t_dbCon.cursor()
  for t_el in v_opt:
    t_dbCur.execute(t_el)
  t_dbCon.commit()
  t_dbCur.close()
  t_dbCon.close()
  return(0)
  
class c_xmlDataParser():
  m_atttrib={'maxDepth':0,'numIed':0,'numAddress':0}
  m_xmlData={'attrib':m_atttrib}
  m_depth=0
  m_path=[]
  m_tag=''
  m_attrib=''
  m_data=''
  m_currentParser=''
  m_recoder={}
  m_gse={}
  m_smv={}
  m_gseCb={}
  m_smvCb={}
  m_dataSet={}
  m_inputs=[]
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
      t_index='{0}{1}/LLN0$go${2}'.format(self.m_recoder['iedName'],self.m_recoder['ldInst'],self.m_recoder['cbName'])
      self.m_gse[t_index]=self.m_recoder
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
      t_index='{0}{1}/LLN0$go${2}'.format(self.m_recoder['iedName'],self.m_recoder['ldInst'],self.m_recoder['cbName'])
      self.m_smv[t_index]=self.m_recoder
      self.m_recoder={}

  def parseDataSet(self,v_mode):
    if(v_mode==0):
      self.m_recoder['name']=self.m_path[-1][1]['name']
      self.m_recoder['lnClass']=self.m_path[-2][1]['lnClass']
      self.m_recoder['ld']=self.m_path[-3][1]['inst']
      self.m_recoder['ap']=self.m_path[-5][1]['name']
      self.m_recoder['ied']=self.m_path[-6][1]['name']
      self.m_recoder['fcda']=[]
    elif(v_mode==1):
      if(self.m_tag=='FCDA'):
         self.m_recoder['fcda'].append(self.m_attrib)
    elif(v_mode==2):
      t_index='{0}{1}/LLN0${2}'.format(self.m_recoder['ied'],self.m_recoder['ld'],self.m_recoder['name'])
      self.m_dataSet[t_index]=self.m_recoder
      self.m_recoder={}
    
  def parseInputs(self,v_mode):
    if(v_mode==0):
      self.m_recoder['lnClass']=self.m_path[-2][1]['lnClass']
      self.m_recoder['ld']=self.m_path[-3][1]['inst']
      self.m_recoder['ap']=self.m_path[-5][1]['name']
      self.m_recoder['ied']=self.m_path[-6][1]['name']
      self.m_recoder['extRef']=[]
    elif(v_mode==1):
      if(self.m_tag=='ExtRef'):
        self.m_recoder['extRef'].append(self.m_attrib)
    elif(v_mode==2):
      self.m_inputs.append(self.m_recoder)
      self.m_recoder={}
        
  def parseGooseCb(self,v_mode):
    if(v_mode==0):
      self.m_recoder['name']=self.m_path[-1][1]['name']
      self.m_recoder['dataSet']=self.m_path[-1][1]['datSet']
      self.m_recoder['ld']=self.m_path[-3][1]['inst']
      self.m_recoder['ap']=self.m_path[-5][1]['name']
      self.m_recoder['ied']=self.m_path[-6][1]['name']
    elif(v_mode==1):
      pass
    elif(v_mode==2):
      t_index='{0}{1}/LLN0$go${2}'.format(self.m_recoder['ied'],self.m_recoder['ld'],self.m_recoder['name'])
      self.m_gseCb[t_index]=self.m_recoder
      self.m_recoder={}
 
  def parseSmvCb(self,v_mode):
    if(v_mode==0):
      self.m_recoder['name']=self.m_path[-1][1]['name']
      self.m_recoder['dataSet']=self.m_path[-1][1]['datSet']
      self.m_recoder['smpRate']=self.m_path[-1][1]['smpRate']
      self.m_recoder['nofASDU']=self.m_path[-1][1]['nofASDU']
      self.m_recoder['ld']=self.m_path[-3][1]['inst']
      self.m_recoder['ap']=self.m_path[-5][1]['name']
      self.m_recoder['ied']=self.m_path[-6][1]['name']
      self.m_recoder['smvOpts']=[]
    elif(v_mode==1):
      if(self.m_tag=='SmvOpts'):
        self.m_recoder['smvOpts'].append(self.m_attrib)
    elif(v_mode==2):
      t_index='{0}{1}/LLN0$go${2}'.format(self.m_recoder['ied'],self.m_recoder['ld'],self.m_recoder['name'])
      self.m_smvCb[t_index]=self.m_recoder
      self.m_recoder={}
 
  def registerTagParser(self):
    self.m_tagParser['GSE']=self.parseGoose
    self.m_tagParser['SMV']=self.parseSmv
    self.m_tagParser['DataSet']=self.parseDataSet
    self.m_tagParser['Inputs']=self.parseInputs 
    self.m_tagParser['GSEControl']=self.parseGooseCb
    self.m_tagParser['SampledValueControl']=self.parseSmvCb   
  
  def __init__(self):
    self.registerTagParser()
  
  def popNamespace(self,v_tag):
    t_tag=v_tag.split('}')[-1]
    return(t_tag)
  
  def start(self,v_tag,v_attrib): 
    self.m_tag=self.popNamespace(v_tag)
    self.m_attrib=v_attrib
    self.m_path.append((self.m_tag,self.m_attrib))
    self.m_depth+=1
    if(self.m_depth>self.m_xmlData['attrib']['maxDepth']):
      self.m_xmlData['attrib']['maxDepth']=self.m_depth
    if(self.m_tag=='IED'):
      self.m_xmlData['attrib']['numIed']+=1
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
    if(self.m_currentParser!='' and v_data!='\n'):
      self.m_data=v_data.strip()
      self.m_tagParser[self.m_currentParser](1)
    
  def close(self):   
    return((self.m_xmlData,self.m_gse,self.m_smv,self.m_gseCb,self.m_smvCb,self.m_dataSet,self.m_inputs))

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

def createGooseTab(v_address,v_cb,v_vtTab):
  t_sn=1
  t_opt=[]
  t_opt.append('create table if not exists \'{0}\'(sn integer primary key NOT NULL,appid integer,vlanid integer,vlanpriority integer,mac varchar,dataset varchar,cb varchar,inputs varchar);'.format(g_gooseTab))
  for t_cb in v_address:
    t_appid=str(int(v_address[t_cb]['APPID'],16))
    t_vlanid=str(int(v_address[t_cb]['VLAN-ID'],16))
    t_vlanpriority=str(int(v_address[t_cb]['VLAN-PRIORITY'],16))
    t_mac=v_address[t_cb]['MAC-Address']
    t_dataset='{0}${1}'.format((t_cb.split('$'))[0],v_cb[t_cb]['dataSet'])
    t_inputs=v_vtTab.get(t_dataset,'')
    t_opt.append('replace into \'{0}\'(sn,appid,vlanid,vlanpriority,mac,dataset,cb,inputs) values({1},{2},{3},{4},\'{5}\',\'{6}\',\'{7}\',\'{8}\');'.format(g_gooseTab,t_sn,t_appid,t_vlanid,t_vlanpriority,t_mac,t_dataset,t_cb,t_inputs))
    t_sn+=1
  sqliteOptBatch(g_dbPath,t_opt)
  
def createSmvTab(v_address,v_cb,v_vtTab):
  t_sn=1
  t_opt=[]
  t_opt.append('create table if not exists \'{0}\'(sn integer primary key NOT NULL,appid integer,vlanid integer,vlanpriority integer,mac varchar,dataset varchar,cb varchar,nofasdu integer,inputs varchar);'.format(g_smvTab))
  for t_cb in v_address:
    t_appid=str(int(v_address[t_cb]['APPID'],16))
    t_vlanid=str(int(v_address[t_cb]['VLAN-ID'],16))
    t_vlanpriority=str(int(v_address[t_cb]['VLAN-PRIORITY'],16))
    t_mac=v_address[t_cb]['MAC-Address']
    t_dataset='{0}${1}'.format((t_cb.split('$'))[0],v_cb[t_cb]['dataSet'])
    t_nofasdu=str(int((v_cb[t_cb]['nofASDU'])))
    t_inputs=v_vtTab.get(t_dataset,'')
    t_opt.append('replace into \'{0}\'(sn,appid,vlanid,vlanpriority,mac,dataset,cb,nofasdu,inputs) values({1},{2},{3},{4},\'{5}\',\'{6}\',\'{7}\',{8},\'{9}\');'.format(g_smvTab,t_sn,t_appid,t_vlanid,t_vlanpriority,t_mac,t_dataset,t_cb,t_nofasdu,t_inputs))
    t_sn+=1
  sqliteOptBatch(g_dbPath,t_opt)

def vtMatch(v_extref,v_fcda):
  t_rt=True
  for el in g_vtMatch:
    if(v_extref.get(el,'')!=v_fcda.get(el,'')):
      t_rt=False
      break
  return(t_rt)
  
def createVtTab(v_address,v_cb,v_dataset,v_inputs):
  t_vtTab={}
  t_sn=1
  t_opt=[]
  for el0 in v_inputs:
    for el1 in el0['extRef']:
      for el2 in v_dataset:
        if(el2.split('/')[0]!='{0}{1}'.format(el1['iedName'],el1['ldInst'])):
          continue
        for el3 in v_dataset[el2]['fcda']:
          if(vtMatch(el1,el3)):
            t_key='{0}{1}/LLN0${2}'.format(v_dataset[el2]['ied'],v_dataset[el2]['ld'],v_dataset[el2]['name'])
            t_value=el0['ied']
            t_vtTab[t_key]=t_vtTab.get(t_key,'')+t_value+','
            t_sn+=1
            break
          else:
            continue
        else:
            continue
        break
      else:
        logging.debug(' vt: no match [%s.%s/%s%s.%s.%s]'%(el1['iedName'],el1['ldInst'],el1['lnClass'],el1['lnInst'],el1['doName'],el1.get('daName',''),))
  return(t_vtTab)

def myTest(v_ds,v_in):
  for t_key in v_ds:
    t_ld=v_ds[t_key]['ld']
    for el0 in v_ds[t_key]['fcda']:
      if(el0['ldInst']!=t_ld):
        print(t_ld,el0)
  for el0 in v_in:
    print(el0)
    break
    
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
    t_result=t_xmlParser.close()
    logging.debug(' %s'%(str(t_result[0]),))
    logging.debug(' smv: %d'%(len(t_result[2]),))
    logging.debug(' goosecb: %d'%(len(t_result[3]),))
    logging.debug(' smvcb: %d'%(len(t_result[4]),)) 
    logging.debug(' dataset: %d'%(len(t_result[5]),)) 
    logging.debug(' inputs: %d'%(len(t_result[6]),)) 
    #myTest(t_result[5],t_result[6])
    t_vtTab=createVtTab(t_result[2],t_result[4],t_result[5],t_result[6])
    createGooseTab(t_result[1],t_result[3],t_vtTab)
    createSmvTab(t_result[2],t_result[4],t_vtTab)
    
if(__name__=='__main__'):
  g_zipFile=sys.argv[1] if(len(sys.argv)>1) else g_zipFile
  logging.debug(' %s'%(g_zipFile,))
  main(g_zipFile,g_xmlExtendName)
