/*
*  main.h
*  by pioevh@163.com 20170320
*  g_    global value
*  m_    member value
*  p_    point value
*  DEF_  macro define
*  s_    struct declear
*/

#pragma once
#pragma pack(1)

#ifndef __mainH__
#define __mainH__

//include files

#include "sys/socket.h"
#include "sys/ioctl.h"
#include "linux/if_packet.h"
#include "linux/if_ether.h"
#include "linux/if_arp.h"
#include "arpa/inet.h"
#include "unistd.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"
#include "stdio.h"
#include "time.h"
#include "sys/time.h"
#include "signal.h"
#include "pthread.h"

#include "rawsocket.h"
#include "sometools.h"
#include "svpub.h"
#include "goosepub.h"

//  macro define

#define DEF_timeAccurSec 0
#define DEF_timeAccurUSec 1

#define DEF_interfaceName "eno33554984"

//  struct

struct s_threadData
{
  int index;
  int m_threadId;
  struct s_threadData* mp_next;
}__attribute__((aligned(1)));

struct s_appData
{
  int m_running;
  long long m_timerCount;
  struct s_ethernetSocket* mp_socket;
  struct s_threadData* mp_threadHead;
}__attribute__((aligned(1)));

//  global

static struct s_appData* gp_appData = NULL;

//  function

void signalintHandler(void);

void signalTimerHandler(void);

void setTimer(int, int);

void createAppData();

void destoryAppData(struct s_appData*);

void work(void);

void test(void);

int main(int, char**);

#endif