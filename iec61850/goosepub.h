/*
*  goosepub.h
*  by pioevh@163.com 20170320
*/

#pragma once
#pragma pack(1)

#ifndef __goosepubH__
#define __goosepubH__

//include files

#include "sys/time.h"
#include "stdint.h"

#include "rawsocket.h"
#include "sometools.h"

//  macro define

#define DEF_gooseDefaultDMac {0x01,0x0c,0xcd,0x01,0x00,0x01}
#define DEF_gooseDefaultPriority 4
#define DEF_gooseDefaultVlanId 1
#define DEF_gooseDefaultAppid 0x0001

//  struct

struct s_goosePublisher
{
  char* mp_buffer;
  char m_dAddr[DEF_macAddrLen];
  char m_sAddr[DEF_macAddrLen];
  char m_priority;
  uint16_t m_vlanId;
  uint16_t m_appId;
  int m_lengthField;
  int m_payloadStart;
  char* m_goID;
  char* m_goCBRef;
  char* m_dataSetRef;
  uint16_t m_minTime;
  uint16_t m_maxTime;
  int m_fixedOffs;
  uint32_t m_confRev;
  uint32_t m_stNum;
  uint32_t m_sqNum;
  uint32_t m_timeAllowedToLive;
  int m_needsCommission;
  int m_simulation;
  struct timeval m_timestamp;
  struct s_appData* mp_appData;
}__attribute__((aligned(1)));

//  global

//  function

#endif