/*
*  61850.h
*  by pioevh@163.com 20170322
*/

#pragma once
#pragma pack(1)

#ifndef __61850H__
#define __61850H__

//include files

#include "rawsocket.h"
#include "sometools.h"

//  macro define

#define DEF_string 256

#define DEF_pduLength(v_int) (((v_int) > 0x00ffffff) ? 4 : (((v_int) > 0x0000ffff) ? 3 : (((v_int) > 0x000000ff) ? 2 : 1)))
#define DEF_actulLength(v_int) (((v_int) > 0x007fffff) ? 4 : (((v_int) > 0x00007fff) ? 3 : (((v_int) > 0x0000007f) ? 2 : 1)))
#define DEF_padLengthHead(v_int) ((((v_int) > 0x0000007f) ? 1 : 0))
#define DEF_modIntEndian(v_int) (((((v_int) & 0xff000000) >> 24) + (((v_int) & 0x00ff0000) >> 8) + (((v_int) & 0x0000ff00) << 8) + ((v_int & 0x000000ff) << 24)))

//  struct

struct s_dataSetNode
{
  unsigned char m_type;
  int m_length;
  void* m_data;
}__attribute__((aligned(1)));

struct s_gooseAndSvThreadData
{
  int m_index;
  int m_threadId;
  long long* mp_timerCount;
  int* m_running;
  struct s_ethernetSocket* mp_socket;
  int m_cmd;
  void* mp_value;
  int m_length;
}__attribute__((aligned(1)));

//  global

//  function

int getTlvValueSize(const char*);

int setTlvTotal(char, const char*, char*, int);

int setTlvLength(int, char*, int);

void setDataTimeToUtc(struct timeval*);

#endif