/*
*  svpub.h
*  by pioevh@163.com 20170320
*/

#pragma once
#pragma pack(1)

#ifndef __svpubH__
#define __svpubH__

//include files

#include "61850.h"
#include "rawsocket.h"
#include "sometools.h"
#include "goosepub.h"

#include "sys/time.h"
#include "stdint.h"
#include "unistd.h"

//  macro define

#define DEF_svDataFileInfo 2048
#define DEF_svDataFilePath "./data/"
#define DEF_svDataFileExtNmae ".rec"
#define DEF_svDataItemLen 8
#define DEF_svDataFormat "type:%[^;];id:%[^;];phase:%[^;];unit:%[^;];factorA:%f;factorB:%f;samplingRate:%d;samplingNumber:%d;"

#define DEF_svDefaultIntervalPerFrame 1
#define DEF_svDefaultDMac {(char)0x01,(char)0x0c,(char)0xcd,(char)0x04,(char)0x00,(char)0x01}
//#define DEF_svDefaultDMac {(char)0xff,(char)0xff,(char)0xff,(char)0xff,(char)0xff,(char)0xff}
#define DEF_svDefaultPriority 4
#define DEF_svDefaultVlanId 1
#define DEF_svDefaultAppid 0x4001
#define DEF_svCmdType 256

#define DEF_asdu 0x00000030

//  struct

//type:A;id:IA_TF7;phase:A;unit:A;factorA:1.165048;factorB:0.000000;samplingRate:5760;samplingNumber:23040;
struct s_svAsduNode
{
  int m_length;
  int m_dataLength;
  int m_smpCount; /* sample counter - reset by sync */
  int m_confRev; /* Configuration revision according to CB */
  int m_smpSynch; /* Synchronization status */
  int m_smpRate;
  char m_type[DEF_svDataItemLen];
  char m_id[DEF_svDataFileInfo];
  char m_phase[DEF_svDataItemLen];
  char m_unit[DEF_svDataItemLen];
  float m_factorA;
  float m_factorB;
  int m_samplingRate;
  int m_samplingNumber;
  int m_txPoint;
  int m_channelNum;
  int (*mp_data)[];
};

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
  int m_numAsdu;
  struct s_linkList* mp_asduHead;
  int m_asduLength;
  uint64_t m_refreshTime; /* local buffer refresh time */
  struct timeval m_timestamp;
}__attribute__((aligned(1)));

//  global

void(*g_svDataModify[DEF_svCmdType])(struct s_svPublisher*, void*, int);

//  function

void svCmdReg(void**);

void svPublisherSetAsdu(struct s_svPublisher*, char*, int);

void svPublisherSetVlanId(struct s_svPublisher*, char*, int);

void svPublisherSetAppid(struct s_svPublisher*, char*, int);

void svPublisherSetEnable(struct s_svPublisher*, char*, int);

void asduListParse(struct s_svPublisher*,char*);

void svDataAsduListDestory(struct s_linkList*);

void svPubDestory(struct s_svPublisher*);

void svFrameSend(struct s_gooseAndSvThreadData*, struct s_svPublisher*);

void svDataUpdate(struct s_gooseAndSvThreadData*, struct s_svPublisher*);

void svBufferPrepare(struct s_svPublisher*, struct s_gooseAndSvThreadData*);

struct s_svPublisher* svPubCreate(struct s_gooseAndSvThreadData*);

int svHeadCreate(struct s_svPublisher*);

struct s_svAsduNode* setAsduNode(char*);

int setSvApduLength(char, int, char*, int);

int setSvApduTlvInt(char, int, char*, int);

int setAsduInt(int, char*, int);

int setSvTlvString(char, char*, char*, int);

int setSvAsduList(struct s_linkList*, char*, int);

int setSvAsduData(struct s_svAsduNode*, char*, int);

int getAsduNodeLength(struct s_svPublisher*, struct s_svAsduNode*);

int getSvAsdulength(struct s_svPublisher*);

void getAnalogValueInfo(struct s_svAsduNode*, char*);

void getAnalogValueData(struct s_svAsduNode*, FILE*, int, int);

int svPduEncode(struct s_svPublisher*, int);

int svPayloadCreate(struct s_svPublisher*);

void svThreadRun(struct s_gooseAndSvThreadData*);

#endif