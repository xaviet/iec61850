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

#include "rawsocket.h"
#include "sometools.h"

//  macro define

#define DEF_svDefaultDMac {0x01,0x0c,0xcd,0x04,0x00,0x01}
#define DEF_svDefaultPriority 4
#define DEF_svDefaultVlanId 1
#define DEF_svDefaultAppid 0x4001

//  struct

struct s_svPublisher
{
  char* mp_buffer;
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
  struct s_appData* mp_appData;
}__attribute__((aligned(1)));

//  global

//  function

#endif