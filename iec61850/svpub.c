/*
*  svpub.c
*  by pioevh@163.com 20170320
*/

//include files

#include "svpub.h"

//  function

void svPubDestory(struct s_svPublisher* vp_svPub)
{
    (vp_svPub->m_id == NULL) ? vp_svPub->m_id = NULL : free(vp_svPub->m_id);
    (vp_svPub->m_dataSetRef == NULL) ? vp_svPub->m_dataSetRef = NULL : free(vp_svPub->m_dataSetRef);
    printf("service appid#%-4d stop\n", vp_svPub->m_appId);
    free(vp_svPub->mp_buffer);
    free(vp_svPub);
}

void svFrameSend(struct s_gooseAndSvThreadData* vp_svThreadData, struct s_svPublisher* vp_svPub)
{
  if (((*vp_svThreadData->mp_timerCount) > (vp_svPub->m_lastTimerCount + vp_svPub->m_frameInterval)) || (vp_svPub->m_lastTimerCount == 0))
  {
    vp_svPub->m_lastTimerCount = *vp_svThreadData->mp_timerCount;
    goosePayloadCreate(vp_svPub);
    sendData(vp_svThreadData->mp_socket, vp_svPub->mp_buffer, vp_svPub->m_length);
    if (vp_svPub->m_lastTimerCount == 0)
    {
      sleep(0);
    }
  }
}

void svThreadRun(struct s_gooseAndSvThreadData* vp_svThreadData)
{
  struct s_svPublisher* tp_svPub = svPubCreate(vp_svThreadData);
  svCmdReg((void**)(&g_svDataModify));
  while (*vp_svThreadData->m_running)
  {
    svDataUpdate(vp_svThreadData, tp_svPub);
    if (tp_svPub->m_enable)
    {
      if (tp_svPub->m_length == 0)
      {
        stNumMod(tp_svPub);
        if (svPayloadCreate(tp_svPub) > DEF_maxFrameLen)
        {
          perror("The sv frame length big than DEF_maxFrameLen");
          break;
        }
      }
      svFrameSend(vp_svThreadData, tp_svPub);
    }
    sleep(0);
  }
  svPubDestory(tp_svPub);
}
