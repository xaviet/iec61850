/*
*  rawsocket.h
*  by pioevh@163.com 20170320
*/

#pragma once
#pragma pack(1)

#ifndef __rawsocketH__
#define __rawsocketH__

//include files

#include "unistd.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "sys/socket.h"
#include "sys/ioctl.h"
#include "linux/if_packet.h"
#include "linux/if_ether.h"
#include "linux/if_arp.h"
#include "arpa/inet.h"

//  macro define

#define DEF_macAddrLen 6
#define DEF_maxFrameLen 1522

//  struct

struct s_ethernetSocket
{
  int m_rawSocket;
  int m_isBind;
  struct sockaddr_ll m_socketAddress;
  char m_interfaceMacAddr[DEF_macAddrLen];
}__attribute__((aligned(1)));

//  global

//  function

void getInterfaceAddress(int, char*, char*);

int getInterfaceIndex(int, char*);

struct s_ethernetSocket* createSocket(char*, char*);

int receiveData(struct s_ethernetSocket*, char*, int);

void sendData(struct s_ethernetSocket*, char*, int);

void destorySocket(struct s_ethernetSocket*);

#endif