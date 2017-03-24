/*
*  svpub.h
*  by pioevh@163.com 20170320
*/

#pragma once
#pragma pack(1)

#ifndef __svpubH__
#define __svpubH__

//include files

#include "sys/time.h"
#include "stdint.h"

#include "61850.h"
#include "rawsocket.h"
#include "sometools.h"
#include "goosepub.h"

//  macro define

#define DEF_svDefaultFrameInterval 1
#define DEF_svDefaultDMac {(char)0x01,(char)0x0c,(char)0xcd,(char)0x04,(char)0x00,(char)0x01}
#define DEF_svDefaultPriority 4
#define DEF_svDefaultVlanId 1
#define DEF_svDefaultAppid 0x4001
#define DEF_svCmdType 256
//  struct

struct s_svPublisher
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
  char* m_id;
  char* m_dataSetRef; /* date set reference */
  uint16_t m_smpCount; /* sample counter - reset by sync */
  uint32_t m_confRev; /* Configuration revision according to CB */
  char m_smpSynch; /* Synchronization status */
  uint16_t m_smpRate;
  uint64_t m_refreshTime; /* local buffer refresh time */
  struct timeval m_timestamp;
  int m_hasDataSetName; /* optional fields in sv asdu */
  int m_hasRefreshTime;
  int m_hasSampleRate;
}__attribute__((aligned(1)));

//  global

void(*g_svDataModify[DEF_svCmdType])(struct s_svPublisher*, void*, int);

//  function

void svFrameSend(struct s_gooseAndSvThreadData*, struct s_svPublisher*)£»

void svThreadRun(struct s_gooseAndSvThreadData*);

#endif