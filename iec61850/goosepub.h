/*
*  goosepub.h
*  by pioevh@163.com 20170320
*/

#pragma once
#pragma pack(1)

#ifndef __goosepubH__
#define __goosepubH__

//include files

#include "time.h"
#include "sys/time.h"
#include "stdint.h"

#include "61850.h"
#include "rawsocket.h"
#include "sometools.h"

//  macro define

#define DEF_gooseDefaultFrameInterval 5000000
#define DEF_gooseDefaultDMac {(char)0x01,(char)0x0c,(char)0xcd,(char)0x01,(char)0x00,(char)0x01}
#define DEF_gooseDefaultPriority 0x4
#define DEF_gooseDefaultVlanId 0x1
#define DEF_gooseDefaultAppid 0x0001
#define DEF_gooseCmdType 256
#define DEF_gooseTimeAllowedToLive 10000

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
  int m_length;
  struct s_gooseAndSvThreadData* mp_next;
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
  uint32_t m_stNum;
  uint32_t m_sqNum;
  int m_test;
  uint32_t m_confRev;
  uint32_t m_timeAllowedToLive;
  int m_needsCommission;
  
  struct timeval m_dateTime; /* time when stNum is increased */
}__attribute__((aligned(1)));

//  global

//  function

void stNumMod(struct s_goosePublisher*);

void sqNumMod(struct s_goosePublisher*);

void goosePublisherSetEnable(struct s_goosePublisher*, char*, int);

void goosePublisherSetGoID(struct s_goosePublisher*, char*, int);

void goosePublisherSetGoCbRef(struct s_goosePublisher*, char*, int);

void goosePublisherSetDataSetRef(struct s_goosePublisher*, char*, int);

void gooseBufferPrepare(struct s_goosePublisher*, struct s_gooseAndSvThreadData*);

int setGooseTlvLengthValue(int, char*, int);

int setGooseApduLength(char, int, char*, int);

int setGooseTlvString(char, char*, char*, int);

int getTlvIntArrayLength(char*, int);

int setGooseTlvInt(char, uint32_t, char*, int);

int setGooseTlvBoolean(char, int, char*, int);

int setGooseTlvOctet(char, char*, int, char*, int);

struct s_goosePublisher* goosePubCreate(struct s_gooseAndSvThreadData*);

void goosePubDestory(struct s_goosePublisher*);

void goosePayloadCreate(struct s_goosePublisher*);

void gooseCmdReg(void**);

void gooseThreadRun(struct s_gooseAndSvThreadData*);

#endif