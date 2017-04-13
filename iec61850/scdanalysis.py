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
  m_atttrib={'maxDepth':0,'numIed':0}
  m_xmlData={'attrib':m_atttrib}
  m_depth=0

  def start(self,tag,attrib): 
    self.m_depth+=1
    if(self.m_depth>self.m_xmlData['attrib']['maxDepth']):
      self.m_xmlData['attrib']['maxDepth']=self.m_depth
    if(tag=='{http://www.iec.ch/61850/2003/SCL}IED'):
      self.m_xmlData['attrib']['numIed']+=1
      
  def end(self,tag):            
    self.m_depth-=1
    
  def data(self,data):
    pass            
    
  def close(self):   
    return(self.m_xmlData)
        
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
    t_xmlParser=elementTree.XMLParser(target=t_xmlTarget)
    t_xmlParser.feed(t_xmlData)
    t_xmlData=t_xmlParser.close()
    logging.debug(' xml: %s'%(str(t_xmlData),))
  
if(__name__=='__main__'):
  g_zipFile=sys.argv[1] if(len(sys.argv)>1) else g_zipFile
  logging.debug(' %s'%(g_zipFile,))
  main(g_zipFile,g_xmlExtendName)

