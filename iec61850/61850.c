/*
*  61850.c
*  by pioevh@163.com 20170320
*/

//include files

#include "61850.h"

//  function

int getTlvLengthValue(int length)
{
  if (length < 128)
  {
    return(1);
  }
  else if (length < 256)
  {
    return(2);
  }
  else
  {
    return (3);
  }
}

int getTlvValueSize(const char* vp_tlvValue)
{
  if (vp_tlvValue != NULL)
  {
    int t_size = 1;
    int t_length = (int)strlen(vp_tlvValue);
    t_size += getTlvLengthValue(t_length);
    t_size += t_length;
    return (t_size);
  }
  else
  {
    return(2);
  }
}

int setTlvTotal(char v_tag, const char* vp_string, char* vp_buffer, int v_bufPos)
{
  vp_buffer[v_bufPos++] = v_tag;
  if (vp_string != NULL)
  {
    int t_length = (int)strlen(vp_string);
    v_bufPos = (int)setTlvLength(t_length, vp_buffer, v_bufPos);
    for (int t_i = 0; t_i < t_length; t_i++)
    {
      vp_buffer[v_bufPos++] = vp_string[t_i];
    }
  }
  else
  {
    vp_buffer[v_bufPos++] = 0;
  }
  return(v_bufPos);
}

int setTlvLength(int v_length, char* vp_buffer, int v_bufPos)
{
  if (v_length < 128)
  {
    vp_buffer[v_bufPos++] = (char)v_length;
  }
  else if (v_length < 256)
  {
    vp_buffer[v_bufPos++] = (char)0x81;
    vp_buffer[v_bufPos++] = (char)v_length;
  }
  else
  {
    vp_buffer[v_bufPos++] = (char)0x82;
    vp_buffer[v_bufPos++] = (char)(v_length / 256);
    vp_buffer[v_bufPos++] = (char)(v_length % 256);
  }
  return(v_bufPos);
}