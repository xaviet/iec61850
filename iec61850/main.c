/*
*  main.c
*  by pioevh@163.com 20170320
*/

//include files

#include "main.h"

//  function

void sigintHandler(void)
{
  gp_appData->m_running = 0;
}

void signalTimerHandler(void)
{
  ++gp_appData->m_timerCount;
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

void createAppData()
{
  gp_appData = (struct s_appData*)malloc(sizeof(struct s_appData));
  memset(gp_appData, 0, sizeof(struct s_appData));
  gp_appData->mp_socket = createSocket(DEF_interfaceName, NULL);
  gp_appData->m_timerCount = 0;
  gp_appData->m_running = 1;
}

void destoryAppData(struct s_appData* vp_appData)
{
  destorySocket(vp_appData->mp_socket);
  free(vp_appData);
}

void work(void)
{
  createAppData();
  while (gp_appData->m_running)
  {
    sleep(1);
  }
  destoryAppData(gp_appData);
}

void test(void)
{
  signal(SIGINT, (void*)sigintHandler);
  signal(SIGALRM, (void*)signalTimerHandler);
  setTimer(DEF_timeAccurSec, DEF_timeAccurUSec);
}

int main(int argc, char** argv)
{
  (0)?work():test();
  return(0);
}
