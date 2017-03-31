/*
*  sometools.h
*  by pioevh@163.com 20170320
*/

#pragma once
#pragma pack(1)

#ifndef __sometoolsH__
#define __sometoolsH__

//include files

#include "stdio.h"
#include "malloc.h"
#include "string.h"
#include "pthread.h"
#include "signal.h"
#include "sys/time.h"

//  macro define

//  struct

struct s_linkList
{
  void* mp_data;
  struct s_linkList* mp_next;
}__attribute__((aligned(1)));

//  global

long long g_timerCount;
int g_running;

//  function

int memDisp(void*, int);

int createThread(void*, void*);

void getDateTime(struct timeval*);

void signalintHandler();

void signalTimerHandler();

void setTimer(int, int);

void createSignal(int**, long long**, int, int);

char* copyString(char*);

void copyChars(char*, char*, int);

struct s_linkList* linkListCreate(void*);

void linkListAppend(struct s_linkList**, struct s_linkList*);

int strCharCount(char*,char);

int timeDelay(int);

#endif