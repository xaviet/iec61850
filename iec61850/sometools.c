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

void signalintHandler(void)
{
  g_running = 0;
}

void signalTimerHandler(void)
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
    *vp_running = &g_running;
    **vp_running = 1;
    signal(SIGINT, (void*)signalintHandler);
  }
  if (vp_timerCount != NULL)
  {
    *vp_timerCount = &g_timerCount;
    **vp_timerCount = 10;
    signal(SIGALRM, (void*)signalTimerHandler);
    setTimer(v_sec, v_uSec);
  }
}

char* copyString(char* vp_string)
{
  unsigned int t_newStringLength = strlen(vp_string) + 1;
  char* t_newString = (char*)malloc(t_newStringLength);
  memcpy(t_newString, vp_string, t_newStringLength);
  return(t_newString);
}
