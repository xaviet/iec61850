/*
*  goosepub.c
*  by pioevh@163.com 20170320
*/

//include files

#include "goosepub.h"

//  function

void goosePublisherSetEnable(struct s_goosePublisher* vp_gooseData, char* vp_goID)
{
  vp_gooseData->m_enable = (int)*vp_goID;
}

void goosePublisherSetGoID(struct s_goosePublisher* vp_gooseData, char* vp_goID)
{
  vp_gooseData->m_goID = copyString(vp_goID);
}

void goosePublisherSetGoCbRef(struct s_goosePublisher* vp_gooseData, char* vp_goCbRef)
{
  vp_gooseData->m_goCBRef = copyString(vp_goCbRef);
}

void goosePublisherSetDataSetRef(struct s_goosePublisher* vp_gooseData, char* vp_dataSetRef)
{
  vp_gooseData->m_dataSetRef = copyString(vp_dataSetRef);
}

void goosePublisherReset(struct s_goosePublisher* vp_gooseData, char* vp_dataSetRef)
{
  vp_gooseData->m_sqNum = 0;
  vp_gooseData->m_stNum = 1;
}

void gooseBufferPrepare(struct s_goosePublisher* vp_gooseData, struct s_gooseAndSvThreadData* vp_gooseThreadData)
{
  char t_mac[DEF_macAddrLen] = DEF_gooseDefaultDMac;
  memcpy(vp_gooseData->m_dAddr, t_mac, DEF_macAddrLen);
  memcpy(vp_gooseData->m_sAddr, vp_gooseThreadData->mp_socket->m_interfaceMacAddr, DEF_macAddrLen);
  vp_gooseData->m_priority = DEF_gooseDefaultPriority;
  vp_gooseData->m_vlanId = DEF_gooseDefaultVlanId;
  vp_gooseData->m_appId = DEF_gooseDefaultAppid;
  vp_gooseData->mp_buffer = (char*)malloc(DEF_maxFrameLen);
  memset(vp_gooseData->mp_buffer, 0, DEF_maxFrameLen);
  vp_gooseData->m_frameInterval = 5000;
  vp_gooseData->m_lastTimerCount = 0;
  vp_gooseData->m_enable = 0;
  vp_gooseData->m_length = 0;
  vp_gooseData->m_stNum = 0;
  vp_gooseData->m_sqNum = 1;
}

struct s_goosePublisher* goosePubCreate(struct s_gooseAndSvThreadData* vp_gooseThreadData)
{
  struct s_goosePublisher* tp_gooseData = (struct s_goosePublisher*)malloc(sizeof(struct s_goosePublisher));
  memset(tp_gooseData, 0, sizeof(struct s_goosePublisher));
  gooseBufferPrepare(tp_gooseData, (struct s_gooseAndSvThreadData*)vp_gooseThreadData);
  return(tp_gooseData);
}

void goosePubDestory(struct s_goosePublisher* vp_gooseData)
{
  (vp_gooseData->m_goID == NULL) ? vp_gooseData->m_goID = NULL : free(vp_gooseData->m_goID);
  (vp_gooseData->m_goCBRef == NULL) ? vp_gooseData->m_goCBRef = NULL : free(vp_gooseData->m_goCBRef);
  (vp_gooseData->m_dataSetRef == NULL) ? vp_gooseData->m_dataSetRef = NULL : free(vp_gooseData->m_dataSetRef);
  free(vp_gooseData->mp_buffer);
  free(vp_gooseData);
}

void goosePayloadCreate(struct s_goosePublisher* vp_gooseData)
{
  int bufPos = 12;
  memcpy(vp_gooseData->mp_buffer, vp_gooseData->m_dAddr, DEF_macAddrLen);
  memcpy(vp_gooseData->mp_buffer + 6, vp_gooseData->m_sAddr, DEF_macAddrLen);
  /* Priority tag - IEEE 802.1Q */
  vp_gooseData->mp_buffer[bufPos++] = (char)0x81;
  vp_gooseData->mp_buffer[bufPos++] = (char)0x00;
  vp_gooseData->mp_buffer[bufPos++] = (char)((vp_gooseData->m_priority << 5) + (vp_gooseData->m_vlanId / 256)); /* Priority + VLAN-ID */
  vp_gooseData->mp_buffer[bufPos++] = (char)(vp_gooseData->m_vlanId % 256); /* VLAN-ID */
  vp_gooseData->mp_buffer[bufPos++] = (char)0x88; /* EtherType GOOSE */
  vp_gooseData->mp_buffer[bufPos++] = (char)0xB8;
  vp_gooseData->mp_buffer[bufPos++] = DEF_gooseDefaultAppid / 256;
  vp_gooseData->mp_buffer[bufPos++] = DEF_gooseDefaultAppid % 256;
  vp_gooseData->m_lengthField = bufPos;
  vp_gooseData->mp_buffer[bufPos++] = (char)0x00;
  vp_gooseData->mp_buffer[bufPos++] = (char)0x08;
  vp_gooseData->mp_buffer[bufPos++] = (char)0x00;
  vp_gooseData->mp_buffer[bufPos++] = (char)0x00;
  vp_gooseData->mp_buffer[bufPos++] = (char)0x00;
  vp_gooseData->mp_buffer[bufPos++] = (char)0x00;
  vp_gooseData->m_payloadStart = bufPos;
}

void gooseCmdReg(void** vp_gooseDataModify)
{
  for (int t_i=0; t_i<DEF_gooseCmdType;t_i++)
  {
    vp_gooseDataModify[t_i] = NULL;
  }
  vp_gooseDataModify[1] = goosePublisherSetEnable;
  vp_gooseDataModify[101] = goosePublisherSetGoID;
  vp_gooseDataModify[102] = goosePublisherSetGoCbRef;
  vp_gooseDataModify[103] = goosePublisherSetDataSetRef;
}

void gooseThreadRun(struct s_gooseAndSvThreadData* vp_gooseThreadData)
{
  struct s_goosePublisher* t_goosePub = goosePubCreate(vp_gooseThreadData);
  void(*t_gooseDataModify[DEF_gooseCmdType])(struct s_goosePublisher*, void*);
  gooseCmdReg((void**)(&t_gooseDataModify));
  while (vp_gooseThreadData->m_running)
  {
    if (vp_gooseThreadData->m_cmd > 0 && t_gooseDataModify[vp_gooseThreadData->m_cmd]!=NULL)
    {
      t_gooseDataModify[vp_gooseThreadData->m_cmd](t_goosePub, vp_gooseThreadData->mp_value);
      vp_gooseThreadData->m_cmd = 0;
    }
    if (t_goosePub->m_enable)
    {
      if (t_goosePub->m_length == 0)
      {
        goosePayloadCreate(t_goosePub);
      }
      if (*(vp_gooseThreadData->mp_timerCount) > (t_goosePub->m_lastTimerCount + t_goosePub->m_frameInterval))
      {
        t_goosePub->m_lastTimerCount = *(vp_gooseThreadData->mp_timerCount);
        sendData(vp_gooseThreadData->mp_socket, t_goosePub->mp_buffer, t_goosePub->m_length);
      }
    }
  }
  goosePubDestory(t_goosePub);
}