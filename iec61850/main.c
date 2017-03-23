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
  struct s_linkList* tp_threadDataLinkList = vp_appData->mp_threadDataHead;
  struct s_gooseAndSvThreadData* tp_waitFree = NULL;
  while (tp_threadDataLinkList != NULL)
  {
    printf("destoryAppData\n");
    tp_waitFree = tp_threadDataLinkList->mp_data;
    tp_threadDataLinkList = tp_threadDataLinkList->mp_next;
    free(tp_waitFree);
  }
  destorySocket(vp_appData->mp_socket);
  free(vp_appData);
}

struct s_linkList* threadDataCreate()
{
  struct s_gooseAndSvThreadData* tp_threadData = (struct s_gooseAndSvThreadData*)malloc(sizeof(struct s_gooseAndSvThreadData));
  memset(tp_threadData, 0, sizeof(struct s_gooseAndSvThreadData));
  tp_threadData->mp_socket = gp_appData->mp_socket;
  tp_threadData->m_running = gp_appData->mp_running;
  tp_threadData->mp_timerCount = gp_appData->mp_timerCount;
  tp_threadData->m_index = gp_appData->m_threadCounter++;
  struct s_linkList* tp_linkList = linkListCreate(tp_threadData);
  return(tp_linkList);
}

void threadDataAppend(struct s_appData* vp_appData, struct s_linkList* vp_threadDataLinkList)
{
  linkListAppend(&vp_appData->mp_threadDataHead, vp_threadDataLinkList);
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
  struct s_linkList* tp_threadDataLinkList = threadDataCreate();
  threadDataAppend(gp_appData, tp_threadDataLinkList);
  createThread(gooseThreadRun, tp_threadDataLinkList->mp_data);
  gooseAndSvPubMod((struct s_gooseAndSvThreadData*)tp_threadDataLinkList->mp_data, 101, "ML2201AMUGO/LLN0$GO$gocb1", 0);
  gooseAndSvPubMod((struct s_gooseAndSvThreadData*)tp_threadDataLinkList->mp_data, 102, "ML2201AMUGO/LLN0$GO$gocb1", 0);
  gooseAndSvPubMod((struct s_gooseAndSvThreadData*)tp_threadDataLinkList->mp_data, 103, "ML2201AMUGO/LLN0$dsGOOSE", 0);
  gooseAndSvPubMod((struct s_gooseAndSvThreadData*)tp_threadDataLinkList->mp_data, 104, NULL, 0x76543210);
  gooseAndSvPubMod((struct s_gooseAndSvThreadData*)tp_threadDataLinkList->mp_data, 104, NULL, 0x00010203);
  gooseAndSvPubMod((struct s_gooseAndSvThreadData*)tp_threadDataLinkList->mp_data, 104, NULL, 0x00000000);
  gooseAndSvPubMod((struct s_gooseAndSvThreadData*)tp_threadDataLinkList->mp_data, 1, NULL, 1);
}

void work()
{
  createSignal((int**)&gp_appData->mp_running, (long long**)&gp_appData->mp_timerCount, DEF_timeAccurSec, DEF_timeAccurUSec);
  gooseAndSvPubCreate();
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
  createSignal((int**)&gp_appData->mp_running, NULL, 0, 0);
  gp_appData->mp_timerCount = (long long*)&g_timerCount;
  gooseAndSvPubCreate();
  while (*gp_appData->mp_running)
  {
    sleep(1);
  }
}
