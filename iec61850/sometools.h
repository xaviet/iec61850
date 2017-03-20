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
#include "pthread.h"
#include "signal.h"
#include "sys/time.h"

//  macro define

//  struct

//  global

//  function

int memDisp(void*, int);

int createThread(void*, void*);

void getDateTime(struct timeval*);

#endif