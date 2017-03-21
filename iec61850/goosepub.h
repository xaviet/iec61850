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

#define DEF_gooseDefaultDMac {(char)0x01,(char)0x0c,(char)0xcd,(char)0x01,(char)0x00,(char)0x01}
#define DEF_gooseDefaultPriority 4
#define DEF_gooseDefaultVlanId 1
#define DEF_gooseDefaultAppid 0x0001
#define DEF_gooseCmdType 256

//  struct

struct s_gooseAndSvThreadData
{
  int m_index;
  int m_threadId;
  long long* mp_timerCount;
  int m_running;
  struct s_ethernetSocket* mp_socket;
  int m_cmd;
  void* mp_value;
  struct s_gooseThreadData* mp_last;
  struct s_gooseThreadData* mp_next;
}__attribute__((aligned(1)));

struct s_goosePublisher
{
  char* mp_buffer;
  int m_frameInterval;
  long long m_lastTimerCount;
  int m_enable;
  int m_length;
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
}__attribute__((aligned(1)));

//  global

//  function

void goosePublisherSetEnable(struct s_goosePublisher*, char*);

void goosePublisherSetGoID(struct s_goosePublisher*, char*);

void goosePublisherSetGoCbRef(struct s_goosePublisher*, char*);

void goosePublisherSetDataSetRef(struct s_goosePublisher*, char*);

void gooseBufferPrepare(struct s_goosePublisher*, struct s_gooseAndSvThreadData*);

void goosePubReset(struct s_goosePublisher*);

struct s_goosePublisher* goosePubCreate(struct s_gooseAndSvThreadData*);

void goosePubDestory(struct s_goosePublisher*);

void goosePayloadCreate();

void gooseCmdReg(void**);

void gooseThreadRun(struct s_gooseAndSvThreadData*);

#endif