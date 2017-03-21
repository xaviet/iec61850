/*
*  main.c
*  by pioevh@163.com 20170320
*/

//include files

#include "main.h"

//  function

void createAppData()
{
  gp_appData = (struct s_appData*)malloc(sizeof(struct s_appData));
  memset(gp_appData, 0, sizeof(struct s_appData));
  gp_appData->mp_socket = createSocket(DEF_interfaceName, NULL);
}

void destoryAppData(struct s_appData* vp_appData)
{
  destorySocket(vp_appData->mp_socket);
  free(vp_appData);
}

void work(void)
{
  createSignal(&gp_appData->mp_running, &gp_appData->mp_timerCount, DEF_timeAccurSec, DEF_timeAccurUSec);
  while (*gp_appData->mp_running)
  {
    sleep(1);
  }
}

#define DEF_test 1

int main(int argc, char** argv)
{
  createAppData();
  (DEF_test)? test():work();
  destoryAppData(gp_appData);
  return(0);
}

void test(void)
{
  createSignal(&gp_appData->mp_running, NULL, 0, 0);
  while (*gp_appData->mp_running)
  {
    sleep(1);
  }
}
