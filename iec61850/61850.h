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

#define DEF_actulLength(v_int) (v_int>0xFF)?2:1

//  struct

//  global

//  function

int getTlvLengthValue(int);

int getTlvValueSize(const char*);

int setTlvTotal(char, const char*, char*, int);

int setTlvLength(int, char*, int);

#endif