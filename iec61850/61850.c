/*
*  61850.c
*  by pioevh@163.com 20170320
*/

//include files

#include "61850.h"

//  function

int getTlvValueSize(const char* vp_tlvValue)
{
  if (vp_tlvValue != NULL)
  {
    int t_size = 1;
    int t_length = (int)strlen(vp_tlvValue);
    t_size += DEF_actulLength(t_length);
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

void setDataTimeToUtc(struct timeval* vp_time)
{
  vp_time->tv_usec *= 1000;
  char t_ch = 0;
  char* tp_octet = (char*)vp_time;
  for (int t_i = 0; t_i <= 4; t_i += 4) //  endian
  {
    t_ch = *(tp_octet + t_i);
    *(tp_octet + t_i) = *(tp_octet + t_i + 3);
    *(tp_octet + t_i + 3) = t_ch;
    t_ch = *(tp_octet + t_i + 1);
    *(tp_octet + t_i + 1) = *(tp_octet + t_i + 2);
    *(tp_octet + t_i + 2) = t_ch;
  }
}
