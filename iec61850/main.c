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
    tp_waitFree = tp_threadDataLinkList->mp_data;
    printf("destoryAppData: %4d\n", tp_waitFree->m_index);
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

void gooseCreate(int v_id, void* vp_gooseThreadRun)
{
  struct s_linkList* tp_threadDataLinkList = threadDataCreate();
  printf("goosePubCreate: %4d\n", ((struct s_gooseAndSvThreadData*)(tp_threadDataLinkList->mp_data))->m_index);
  threadDataAppend(gp_appData, tp_threadDataLinkList);
  createThread(vp_gooseThreadRun, tp_threadDataLinkList->mp_data);
  char t_string[128] = { 0 };
  sprintf(t_string, "%s%d", "ML2201AMUGO/LLN0$GO$gocb", v_id);
  gooseAndSvPubMod((struct s_gooseAndSvThreadData*)tp_threadDataLinkList->mp_data, 101, NULL, v_id);
  gooseAndSvPubMod((struct s_gooseAndSvThreadData*)tp_threadDataLinkList->mp_data, 102, NULL, v_id);
  gooseAndSvPubMod((struct s_gooseAndSvThreadData*)tp_threadDataLinkList->mp_data, 103, t_string, 0);
  gooseAndSvPubMod((struct s_gooseAndSvThreadData*)tp_threadDataLinkList->mp_data, 104, "PE2201APIGO/LLN0$GO$GoCBTrip", 0);
  gooseAndSvPubMod((struct s_gooseAndSvThreadData*)tp_threadDataLinkList->mp_data, 105, "PE2201APIGO/LLN100$GO$GoCBTripdsGOOSE", 0);
  for (int t_i = 0; t_i < v_id; t_i++)
  {
    gooseAndSvPubMod((struct s_gooseAndSvThreadData*)tp_threadDataLinkList->mp_data, 106, NULL, t_i);
  }
  gooseAndSvPubMod((struct s_gooseAndSvThreadData*)tp_threadDataLinkList->mp_data, 1, NULL, 1);
}

void svCreate(int v_id, void* vp_svThreadRun, char* v_data)
{
  struct s_linkList* tp_threadDataLinkList = threadDataCreate();
  printf("svPubCreate: %4d\n", ((struct s_gooseAndSvThreadData*)(tp_threadDataLinkList->mp_data))->m_index);
  threadDataAppend(gp_appData, tp_threadDataLinkList);
  createThread(vp_svThreadRun, tp_threadDataLinkList->mp_data);
  gooseAndSvPubMod((struct s_gooseAndSvThreadData*)tp_threadDataLinkList->mp_data, 101, NULL, 0x4000 + v_id);
  gooseAndSvPubMod((struct s_gooseAndSvThreadData*)tp_threadDataLinkList->mp_data, 102, NULL, 4000 + v_id);
  gooseAndSvPubMod((struct s_gooseAndSvThreadData*)tp_threadDataLinkList->mp_data, 103, v_data, 0);
  gooseAndSvPubMod((struct s_gooseAndSvThreadData*)tp_threadDataLinkList->mp_data, 1, NULL, 1);
}

void pubCreate(int v_goosePubNum, int v_svPubNum)
{
  char t_dataList[DEF_svDataFileInfo] = "";
  int t_i = 0;
  for (t_i = 1; t_i <= v_goosePubNum; t_i++)
  {
    gooseCreate(t_i, gooseThreadRun);
  }
  for (t_i = 1; t_i <= v_svPubNum; t_i++)
  {
    sprintf(t_dataList, "IA_G%d,IB_G%d,IC_G%d;VA_G%d,VB_G%d,VC_G%d,VA_G%d,VB_G%d,VC_G%d;", t_i, t_i, t_i, t_i, t_i, t_i, t_i, t_i, t_i);
    if (t_i <= 4)
    {
      svCreate(t_i, svThreadRun, t_dataList);
    }
    else
    {
      break;
    }
  }
}
void work(int v_goosePubNum, int v_svPubNum)
{
  createSignal((int**)&gp_appData->mp_running, (long long**)&gp_appData->mp_timerCount, DEF_timeAccurSec, DEF_timeAccurUSec);
  pubCreate(v_goosePubNum, v_svPubNum);
}

int main(int argc, char** argv)
{
  int t_test = 1;
  int t_goosePubNum = 2;
  int t_svPubNum = 2;
  if (argc > 1)
  {
    sscanf(argv[1], "%d", &t_goosePubNum);
  }
  if (argc > 2)
  {
    sscanf(argv[2], "%d", &t_svPubNum);
  }
  if (argc > 3)
  {
    sscanf(argv[3], "%d", &t_test);
  }
  createAppData();
  (t_test == 1) ? test(t_goosePubNum, t_svPubNum) : work(t_goosePubNum, t_svPubNum);
  g_byteCount = 0;
  int t_timer = 0;
  struct timeval t_startTime, t_currentTime;
  getDateTime(&t_startTime);
  while (*gp_appData->mp_running)
  {
    timeDelay(0x1);
    getDateTime(&t_currentTime);
    t_timer = t_currentTime.tv_sec - t_startTime.tv_sec;
    if (t_timer)
    {
      printf("\rBand: %9.3fMbps Send Byte: %-16lld  ", (float)(g_byteCount) / (((float)t_timer) * 125000), g_byteCount);
    }
  }
  printf("\n");
  destoryAppData(gp_appData);
  return(0);
}

void test(int v_goosePubNum, int v_svPubNum)
{
  createSignal((int**)&gp_appData->mp_running, NULL, 0, 0);
  gp_appData->mp_timerCount = (long long*)&g_timerCount;
  pubCreate(v_goosePubNum, v_svPubNum);
}
