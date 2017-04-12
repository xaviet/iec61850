# /usr/bin/python3
# -*- coding: utf-8 -*-
# scdanalysis.py
# by pioevh@163.com 20170411
# g_    global value
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

g_zipFile='./fengqiao220.zip'
g_xmlFileExt='scd'

def spentTime(v_fun):
  '''
    decorator spentTime
  '''
  def t_decoratorfun(v_para):
    t_starttime=time.time()
    t_rt=v_fun(v_para)
    print('spentTime: '+str(int((time.time()-t_starttime)*1000000))+'us spent.')
    return(t_rt)
  return(t_decoratorfun)

class xmlDataParser():
  pass
  def _init_(self,v_xmlData):
    pass
    
class xmlFile():
  pass
  def _init_(self,v_xmlFile):
    pass
  
class zipFile():
  pass
  def _init_(self,v_zipFile,v_xmlFileExt):
    pass

@spentTime
def main(v_zipFile,v_xmlFileExt):
  t_xmlFile=zipFile(v_zipFile,v_xmlFileExt)
  t_xmlData=xmlFile(t_xmlFile)
  return(0)
  
if(__name__=='__main__'):
  g_zipFile=sys.argv[1] if(len(sys.argv)>1) else g_zipFile
  logging.info('%s'%g_zipFile)
  main(g_zipFile,g_xmlFileExt)
  pdb.set_trace()

