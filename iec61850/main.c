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
  gp_appData->mp_threadDataHead = NULL;
  gp_appData->m_threadCounter = 0;
}

void destoryAppData(struct s_appData* vp_appData)
{
  struct s_gooseAndSvThreadData* tp_threadData = vp_appData->mp_threadDataHead;
  struct s_gooseAndSvThreadData* tp_waitFree = NULL;
  while (tp_threadData != NULL)
  {
    printf("destoryAppData\n");
    tp_threadData->m_running = *vp_appData->mp_running;
    tp_waitFree = tp_threadData;
    tp_threadData = tp_threadData->mp_next;
    free(tp_waitFree);
  }
  destorySocket(vp_appData->mp_socket);
  free(vp_appData);
}

struct s_gooseAndSvThreadData* threadDataCreate()
{
  struct s_gooseAndSvThreadData* tp_threadData = (struct s_gooseAndSvThreadData*)malloc(sizeof(struct s_gooseAndSvThreadData));
  memset(tp_threadData, 0, sizeof(struct s_gooseAndSvThreadData));
  tp_threadData->mp_socket = gp_appData->mp_socket;
  tp_threadData->m_running = *gp_appData->mp_running;
  tp_threadData->m_index = gp_appData->m_threadCounter++;
  return(tp_threadData);
}

void threadDataAppend(struct s_appData* vp_appData, struct s_gooseAndSvThreadData* vp_threadData)
{
  struct s_gooseAndSvThreadData* tp_threadData = vp_appData->mp_threadDataHead;
  if (tp_threadData == NULL)
  {
    vp_appData->mp_threadDataHead = vp_threadData;
  }
  else
  {
    while (tp_threadData->mp_next != NULL)
    {
      tp_threadData = tp_threadData->mp_next;
    }
    tp_threadData->mp_next = vp_threadData;
  }
}

void gooseAndSvPubMod(struct s_gooseAndSvThreadData* vp_threadData, int v_type, char* vp_buff, int v_length)
{
  while (vp_threadData->m_cmd != 0)
  {
    sleep(0);
  }
  vp_threadData->mp_value = vp_buff;
  vp_threadData->m_length = v_length;
  vp_threadData->m_cmd = v_type;
}

void gooseAndSvPubCreate()
{
  printf("gooseAndSvPubCreate\n");
  struct s_gooseAndSvThreadData* tp_threadData = threadDataCreate();
  threadDataAppend(gp_appData, tp_threadData);
  createThread(gooseThreadRun, tp_threadData);
  gooseAndSvPubMod(tp_threadData, 101, "ML2201AMUGO/LLN0$GO$gocb1", 0);
  gooseAndSvPubMod(tp_threadData, 102, "ML2201AMUGO/LLN0$GO$gocb1", 0);
  gooseAndSvPubMod(tp_threadData, 103, "ML2201AMUGO/LLN0$dsGOOSE", 0);
  gooseAndSvPubMod(tp_threadData, 1, NULL, 1);
}

void work()
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
  (DEF_test) ? test() : work();
  destoryAppData(gp_appData);
  return(0);
}

void test()
{
  createSignal(&gp_appData->mp_running, NULL, 0, 0);
  gooseAndSvPubCreate();
  while (*gp_appData->mp_running)
  {
    sleep(1);
  }
}
