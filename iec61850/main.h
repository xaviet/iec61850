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
#define DEF_timeAccurUSec 1000

#define DEF_interfaceName "eth0"

//  struct

struct s_appData
{
  long long* mp_timerCount;
  int* mp_running;
  struct s_ethernetSocket* mp_socket;
  struct s_gooseAndSvThreadData* mp_threadDataHead;
  int m_threadCounter;
}__attribute__((aligned(1)));

//  global

struct s_appData* gp_appData;

//  function

struct s_gooseAndSvThreadData* threadDataCreate();

void gooseAndSvPubCreate();

void threadDataAppend(struct s_appData*, struct s_gooseAndSvThreadData*);

void createAppData();

void destoryAppData(struct s_appData*);

void work();

void test();

int main(int, char**);

#endif