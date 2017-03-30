/*
*  sometools.c
*  by pioevh@163.com 20170320
*/

//include files

#include "sometools.h"

//  function

int memDisp(void* vp_buffer, int v_length)
{
  int t_i = 0;
  char* t_ch = (char*)vp_buffer;
  for (t_i = 0; t_i<v_length; t_i++)
  {
    if (t_i % 16 == 0)
    {
      printf("\n");
    }
    else if (t_i % 8 == 0)
    {
      printf("  ");
    }
    printf("%02x ", (*(t_ch + t_i)) & 0xff);
  }
  printf("\n");
  return(0);
}

int createThread(void* vp_function, void* vp_data)
{
  pthread_t t_thread;
  if (pthread_create(&t_thread, NULL, vp_function, vp_data) != 0)
  {
    perror("Failed to create pthread ");
  }
  return(0);
}

void getDateTime(struct timeval* vp_time)
{
  gettimeofday(vp_time, NULL);
}

void signalintHandler()
{
  g_running = 0;
}

void signalTimerHandler()
{
  g_timerCount++;
}

void setTimer(int v_sec, int v_uSec)
{
  struct itimerval t_itv;
  t_itv.it_interval.tv_sec = v_sec;
  t_itv.it_interval.tv_usec = v_uSec;
  t_itv.it_value.tv_sec = v_sec;
  t_itv.it_value.tv_usec = v_uSec;
  setitimer(ITIMER_REAL, &t_itv, NULL);
}

void createSignal(int** vp_running, long long** vp_timerCount, int v_sec, int v_uSec)
{
  if (vp_running!=NULL)
  {
    *vp_running = (int*)&g_running;
    g_running = 1;
    signal(SIGINT, (void*)signalintHandler);
  }
  if (vp_timerCount != NULL)
  {
    *vp_timerCount = (long long*)&g_timerCount;
    g_timerCount = 0;
    signal(SIGALRM, (void*)signalTimerHandler);
    setTimer(v_sec, v_uSec);
  }
}

char* copyString(char* vp_string)
{
  int t_newStringLength = (int)strlen(vp_string) + 1;
  char* t_newString = (char*)malloc((size_t)t_newStringLength);
  memcpy(t_newString, vp_string, (size_t)t_newStringLength);
  return(t_newString);
}

void copyChars(char* vp_dest, char* vp_src, int v_len)
{
  while (v_len--)
  {
    *vp_dest= *vp_src;
    vp_dest++;
    vp_src++;
  }
}

struct s_linkList* linkListCreate(void* tp_data)
{
  struct s_linkList* tp_linkList = (struct s_linkList*)malloc(sizeof(struct s_linkList));
  memset(tp_linkList, 0, sizeof(struct s_linkList));
  tp_linkList->mp_data = tp_data;
  return(tp_linkList);
}

void linkListAppend(struct s_linkList** vp_head, struct s_linkList* vp_node)
{
  struct s_linkList* tp_node = *vp_head;
  if (tp_node == NULL)
  {
    *vp_head = vp_node;
  }
  else
  {
    while (tp_node->mp_next != NULL)
    {
      tp_node = tp_node->mp_next;
    }
    tp_node->mp_next = vp_node;
  }
}

int strCharCount(char* vp_str, char v_c)
{
  int t_i = 0;
  char* tp_str = vp_str;
  while(1)
  {
    tp_str = strchr(tp_str, v_c);
    if (tp_str)
    {
      t_i++;
      tp_str += 1;
    }
    else
    {
      break;
    }
  }
  return(t_i);
}