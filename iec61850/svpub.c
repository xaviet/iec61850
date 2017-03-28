/*
*  svpub.c
*  by pioevh@163.com 20170320
*/

//include files

#include "svpub.h"

//  function

void svCmdReg(void** vp_svDataModify)
{
  for (int t_i = 0; t_i < DEF_svCmdType; t_i++)
  {
    vp_svDataModify[t_i] = NULL;
  }
  vp_svDataModify[1] = svPublisherSetEnable;
  vp_svDataModify[101] = svPublisherSetAppid;
  vp_svDataModify[102] = svPublisherSetVlanId;
  vp_svDataModify[103] = svPublisherSetAsdu;
}

void svPublisherSetAsdu(struct s_svPublisher* vp_svData, char* vp_asduList, int v_length)
{
  asduListParse(vp_svData, vp_asduList);
}

void svPublisherSetVlanId(struct s_svPublisher* vp_svData, char* vp_goID, int v_length)
{
  vp_svData->m_vlanId = (uint16_t)(v_length & 0xffff);
}

void svPublisherSetAppid(struct s_svPublisher* vp_svData, char* vp_goID, int v_length)
{
  vp_svData->m_appId = (uint16_t)(v_length & 0xffff);
}

void svPublisherSetEnable(struct s_svPublisher* vp_svData, char* vp_goID, int v_length)
{
  vp_svData->m_enable = v_length;
}

struct s_svAsduNode* setAsduNode(char* vp_path)
{
  struct s_svAsduNode* tp_node = (struct s_svAsduNode*)malloc(sizeof(struct s_svAsduNode));
  memset(tp_node, 0, sizeof(struct s_svAsduNode));
  FILE* t_fd = fopen(vp_path, "r");
  char t_buf[DEF_svDataFileInfo] = { 0 };
  if (t_fd)
  {
    char* t_buff = NULL;
    fgets(t_buf, DEF_svDataFileInfo, t_fd);

    fclose(t_fd);
  }
  else
  {
    perror("file open fault");
  }
  return(tp_node);
}

void setSvAsduList(struct s_svPublisher* vp_svData, char* vp_fileName)
{
  char t_path[DEF_svDataFileInfo] = "";
  sprintf(t_path, "%s%s%s", DEF_svDataFilePath, vp_fileName, DEF_svDataFileExtNmae);
  struct s_linkList* tp_linkList = linkListCreate(setAsduNode(t_path));
  linkListAppend(&vp_svData->mp_asduHead, tp_linkList);
  vp_svData->m_numAsdu++;
}

void asduListParse(struct s_svPublisher* vp_svData, char* vp_asduList)
{
  char t_fileName[DEF_svDataItemLen] = "";
  char* t_chBegin = vp_asduList;
  char* t_chEnd = vp_asduList;
  t_chEnd = strchr(t_chBegin, ';');
  while (t_chEnd)
  {
    copyChars(t_fileName, t_chBegin, t_chEnd- t_chBegin);
    t_chBegin = t_chEnd + 1;
    t_chEnd = strchr(t_chBegin, ';');
    setSvAsduList(vp_svData, t_fileName);
    memset(t_fileName, 0, DEF_svDataItemLen);
  }
}

void svDataAsduListDestory(struct s_linkList* vp_dataSetHead)
{
  struct s_linkList* tp_node = vp_dataSetHead;
  struct s_linkList* tp_waitFree = NULL;
  while (tp_node != NULL)
  {
    tp_waitFree = tp_node;
    tp_node = tp_node->mp_next;
    free(((struct s_svAsduNode*)(tp_waitFree->mp_data))->mp_data);
    free(tp_waitFree->mp_data);
    free(tp_waitFree);
  }
}

void svPubDestory(struct s_svPublisher* vp_svPub)
{
    (vp_svPub->m_id == NULL) ? vp_svPub->m_id = NULL : free(vp_svPub->m_id);
    (vp_svPub->m_dataSetRef == NULL) ? vp_svPub->m_dataSetRef = NULL : free(vp_svPub->m_dataSetRef);
    svDataAsduListDestory(vp_svPub->mp_asduHead);
    printf("service appid#%-4d stop\n", vp_svPub->m_appId);
    free(vp_svPub->mp_buffer);
    free(vp_svPub);
}

void svFrameSend(struct s_gooseAndSvThreadData* vp_svThreadData, struct s_svPublisher* vp_svPub)
{
  if (((*vp_svThreadData->mp_timerCount) > (vp_svPub->m_lastTimerCount + vp_svPub->m_frameInterval)) || (vp_svPub->m_lastTimerCount == 0))
  {
    vp_svPub->m_lastTimerCount = *vp_svThreadData->mp_timerCount;
    svPayloadCreate(vp_svPub);
    sendData(vp_svThreadData->mp_socket, vp_svPub->mp_buffer, vp_svPub->m_length);
    if (vp_svPub->m_lastTimerCount == 0)
    {
      sleep(0);
    }
  }
}

void svDataUpdate(struct s_gooseAndSvThreadData* vp_svThreadData , struct s_svPublisher* tp_svPub)
{
  if (vp_svThreadData->m_cmd > 0 && g_svDataModify[vp_svThreadData->m_cmd] != NULL)
  {
    (g_svDataModify[vp_svThreadData->m_cmd])(tp_svPub, vp_svThreadData->mp_value, vp_svThreadData->m_length);
    tp_svPub->m_length = 0;
    vp_svThreadData->m_cmd = 0;
  }
}

void svBufferPrepare(struct s_svPublisher* vp_svData, struct s_gooseAndSvThreadData* vp_svThreadData)
{
  char t_mac[DEF_macAddrLen] = DEF_svDefaultDMac;
  memcpy(vp_svData->m_dAddr, t_mac, DEF_macAddrLen);
  memcpy(vp_svData->m_sAddr, vp_svThreadData->mp_socket->m_interfaceMacAddr, DEF_macAddrLen);
  vp_svData->m_priority = DEF_svDefaultPriority;
  vp_svData->m_vlanId = DEF_svDefaultVlanId;
  vp_svData->m_appId = DEF_svDefaultAppid;
  vp_svData->mp_buffer = (char*)malloc(DEF_maxFrameLen);
  memset(vp_svData->mp_buffer, 0, DEF_maxFrameLen);
  vp_svData->m_frameInterval = DEF_svDefaultFrameInterval;
}

struct s_svPublisher* svPubCreate(struct s_gooseAndSvThreadData* vp_svThreadData)
{
  struct s_svPublisher* tp_svData = (struct s_svPublisher*)malloc(sizeof(struct s_svPublisher));
  memset(tp_svData, 0, sizeof(struct s_svPublisher));
  svBufferPrepare(tp_svData, (struct s_gooseAndSvThreadData*)vp_svThreadData);
  return(tp_svData);
}

int svHeadCreate(struct s_svPublisher* vp_svData)
{
  int t_bufPos = 12;
  memcpy(vp_svData->mp_buffer, vp_svData->m_dAddr, DEF_macAddrLen);
  memcpy(vp_svData->mp_buffer + 6, vp_svData->m_sAddr, DEF_macAddrLen);
  /* Priority tag - IEEE 802.1Q */
  vp_svData->mp_buffer[t_bufPos++] = (char)0x81;
  vp_svData->mp_buffer[t_bufPos++] = (char)0x00;
  vp_svData->mp_buffer[t_bufPos++] = (char)((vp_svData->m_priority << 5) + (vp_svData->m_vlanId / 256)); /* Priority + VLAN-ID */
  vp_svData->mp_buffer[t_bufPos++] = (char)(vp_svData->m_vlanId % 256); /* VLAN-ID */
  vp_svData->mp_buffer[t_bufPos++] = (char)0x88; /* EtherType sv */
  vp_svData->mp_buffer[t_bufPos++] = (char)0xba;
  vp_svData->mp_buffer[t_bufPos++] = (char)(vp_svData->m_appId / 256);
  vp_svData->mp_buffer[t_bufPos++] = (char)(vp_svData->m_appId % 256);
  vp_svData->m_lengthField = t_bufPos;
  vp_svData->mp_buffer[t_bufPos++] = (char)0x00; //  PDU length
  vp_svData->mp_buffer[t_bufPos++] = (char)0x00;
  vp_svData->mp_buffer[t_bufPos++] = (char)0x00; //  PDU reserve
  vp_svData->mp_buffer[t_bufPos++] = (char)0x00;
  vp_svData->mp_buffer[t_bufPos++] = (char)0x00;
  vp_svData->mp_buffer[t_bufPos++] = (char)0x00;
  vp_svData->m_payloadStart = t_bufPos;  //  APDU pos
  //  to be do...

  return(t_bufPos);
}

int setsvApduLength(char v_tag, int v_length, char* vp_buffer, int v_bufPos)
{
  return(setGooseApduLength(v_tag, v_length, vp_buffer, v_bufPos));
}

int svPduEncode(struct s_svPublisher* vp_svData, int v_svApduLength)
{
  int t_bufPos = 0;
  char* t_buff = vp_svData->mp_buffer + vp_svData->m_payloadStart;
  t_bufPos = setsvApduLength((char)0x60, v_svApduLength, t_buff, t_bufPos);
  //  to be do...

  return(t_bufPos);
}

int getSvAllDataLength(struct s_linkList* vp_dataSetHead)
{
  int t_len = 0;
  struct s_linkList* tp_node = vp_dataSetHead;
  struct s_dataSetNode* tp_data = NULL;
  while (tp_node != NULL)
  {
    tp_data = tp_node->mp_data;
    switch ((unsigned int)tp_data->m_type)
    {
    case DEF_asdu:
      break;
    default:
      break;
    }
    tp_node = tp_node->mp_next;
  }
  return(t_len);
}

int svApduCalculate(struct s_svPublisher* vp_svData)
{
  int t_svApduLength = 0;
  if (vp_svData->m_hasDataSetName)
  {
    t_svApduLength += getTlvValueSize(vp_svData->m_dataSetRef);
    t_svApduLength += 4 + 6 + 3 + 4; /* for smpCnt + confRev + smpSynch + smpMod */
  }
  if (vp_svData->m_hasRefreshTime)
  {
    t_svApduLength += 10; /* for refrTim */
  }
  if (vp_svData->m_hasSampleRate)
  {
    t_svApduLength += 4; /* for smpRate */
  }
  vp_svData->m_numAsdu = getSvAllDataLength(vp_svData->mp_asduHead);
  t_svApduLength += 2 + (DEF_actulLength(vp_svData->m_numAsdu));
  //  to be do...

  int t_pduLen = t_svApduLength + (vp_svData->m_payloadStart - vp_svData->m_lengthField);
  vp_svData->mp_buffer[vp_svData->m_lengthField] = (char)(t_pduLen / 256);
  vp_svData->mp_buffer[vp_svData->m_lengthField + 1] = (char)(t_pduLen % 256);
  return(t_svApduLength);
}

int svPayloadCreate(struct s_svPublisher* vp_svData)
{
  svHeadCreate(vp_svData);
  int t_svApduLength = svApduCalculate(vp_svData);
  int t_bufPos = svPduEncode(vp_svData, t_svApduLength);
  vp_svData->m_length = vp_svData->m_payloadStart + t_bufPos;
  return(vp_svData->m_length);
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
