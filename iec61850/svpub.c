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
  vp_svDataModify[104] = svPublisherSetMac;
  vp_svDataModify[105] = svPublisherSetVlanPriority;
  vp_svDataModify[106] = svPublisherSetNofasdu;
}

void svPublisherSetNofasdu(struct s_svPublisher* vp_svData, char* vp_buffer, int v_length)
{
  vp_svData->m_numAsdu = v_length;
}

void svPublisherSetMac(struct s_svPublisher* vp_svData, char* vp_buffer, int v_length)
{
  memcpy(vp_svData->m_dAddr, vp_buffer, DEF_macAddrLen);
}

void svPublisherSetVlanPriority(struct s_svPublisher* vp_svData, char* vp_buffer, int v_length)
{
  vp_svData->m_priority = (char)(v_length & 0x7);
}

void svPublisherSetAsdu(struct s_svPublisher* vp_svData, char* vp_buffer, int v_length)
{
  vp_svData->m_id = copyString(vp_buffer);
}

void svPublisherSetVlanId(struct s_svPublisher* vp_svData, char* vp_buffer, int v_length)
{
  vp_svData->m_vlanId = (uint16_t)(v_length & 0xffff);
}

void svPublisherSetAppid(struct s_svPublisher* vp_svData, char* vp_buffer, int v_length)
{
  vp_svData->m_appId = (uint16_t)(v_length & 0xffff);
}

void svPublisherSetEnable(struct s_svPublisher* vp_svData, char* vp_buffer, int v_length)
{
  asduListSet(vp_svData);
  vp_svData->m_enable = v_length;
}

void getAnalogValueInfo(struct s_svAsduNode* vp_node, char* vp_buf)
{
  char t_buf[DEF_svDataFileInfo] = { 0 };
  sscanf(vp_buf, DEF_svDataFormat, vp_node->m_type, t_buf, vp_node->m_phase, vp_node->m_unit, &vp_node->m_factorA, &vp_node->m_factorB, &vp_node->m_samplingRate, &vp_node->m_samplingNumber);
}

void getAnalogValueData(struct s_svAsduNode* vp_node, FILE* v_fd, int v_channelNum, int v_index)
{
  char t_data[DEF_svDataFileInfo] = { 0 };
  int t_i = v_index;
  int t_smpTotalNumber = vp_node->m_samplingNumber * v_channelNum;
  while (1)
  {
    fgets(t_data, DEF_svDataFileInfo, v_fd);
    if ((strlen(t_data) > 0) && (t_i < t_smpTotalNumber))
    {
      sscanf(t_data, "%d", &(*vp_node->mp_data)[t_i]);
      t_i += v_channelNum;
    }
    else
    {
      break;
    }
  }
}

struct s_svAsduNode* setAsduNode(char* vp_fileName, struct s_svPublisher* vp_svData)
{
  struct s_svAsduNode* tp_node = (struct s_svAsduNode*)malloc(sizeof(struct s_svAsduNode));
  memset(tp_node, 0, sizeof(struct s_svAsduNode));
  strcpy(tp_node->m_id, vp_svData->m_id);
  strcat(vp_fileName, ",");
  tp_node->m_channelNum = strCharCount(vp_fileName, ',');
  char t_path[DEF_svDataFileInfo] = "";
  char t_fileName[DEF_svDataFileInfo] = "";
  char* t_chBegin = vp_fileName;
  char* t_chEnd = NULL;
  char t_buf[DEF_svDataFileInfo] = { 0 };
  unsigned int t_len = 0;
  for (int t_i = 0; t_i < tp_node->m_channelNum; t_i++)
  {
    t_chEnd = strchr(t_chBegin, ',');
    copyChars(t_fileName, t_chBegin, t_chEnd - t_chBegin);
    sprintf(t_path, "%s%s%s", DEF_svDataFilePath, t_fileName, DEF_svDataFileExtNmae);
    FILE* t_fd = fopen(t_path, "r");
    fgets(t_buf, DEF_svDataFileInfo, t_fd);
    if (strstr(t_buf, "type:A;"))
    {
      if (t_i == 0)
      {
        getAnalogValueInfo(tp_node, t_buf);
        tp_node->m_confRev = 1;
        tp_node->m_smpCount = 0;
        tp_node->m_smpSynch = 1;
        t_len = ((unsigned int)tp_node->m_samplingNumber * (unsigned int)tp_node->m_channelNum * sizeof(int));
        tp_node->mp_data = malloc(t_len);
        memset(tp_node->mp_data, 0, t_len);
      }
      getAnalogValueData(tp_node, t_fd, tp_node->m_channelNum, t_i);
      fclose(t_fd);
    }
    else if (strstr(t_buf, "type:D;"))
    {
    }
    else
    {
      perror("file content error");
    }
    t_chBegin = t_chEnd + 1;
  }
  return(tp_node);
}

void setAsduList(struct s_svPublisher* vp_svData, char* vp_fileName)
{
  struct s_linkList* tp_linkList = linkListCreate(setAsduNode(vp_fileName, vp_svData));
  linkListAppend(&vp_svData->mp_asduHead, tp_linkList);
}

void asduListSet(struct s_svPublisher* vp_svData)
{
  char t_asduList[] = "IA_G1;";
  char t_fileName[DEF_svDataFileInfo] = "";
  char* t_chBegin = t_asduList;
  char* t_chEnd = t_asduList;
  t_chEnd = strchr(t_chBegin, ';');
  while (t_chEnd)
  {
    copyChars(t_fileName, t_chBegin, t_chEnd - t_chBegin);
    t_chBegin = t_chEnd + 1;
    t_chEnd = strchr(t_chBegin, ';');
    setAsduList(vp_svData, t_fileName);
    memset(t_fileName, 0, DEF_svDataFileInfo);
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

void svDataUpdate(struct s_gooseAndSvThreadData* vp_svThreadData, struct s_svPublisher* vp_svPub)
{
  if (vp_svThreadData->m_cmd > 0 && g_svDataModify[vp_svThreadData->m_cmd] != NULL)
  {
    (g_svDataModify[vp_svThreadData->m_cmd])(vp_svPub, vp_svThreadData->mp_value, vp_svThreadData->m_length);
    vp_svPub->m_length = 0;
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
  vp_svData->m_frameInterval = DEF_svDefaultIntervalPerFrame;
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
  //vp_svData->m_vlanId = 1;
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
  return(t_bufPos);
}

int setSvApduLength(char v_tag, int v_length, char* vp_buffer, int v_bufPos)
{
  return(setGooseApduLength(v_tag, v_length, vp_buffer, v_bufPos));
}

int setSvTlvString(char v_tag, char* vp_string, char* vp_buffer, int v_bufPos)
{
  return(setGooseTlvString(v_tag, vp_string, vp_buffer, v_bufPos));
}

int setSvApduTlvInt(char v_tag, int v_length, char* vp_buffer, int v_bufPos)
{
  return(setGooseTlvInt(v_tag, (unsigned int)v_length, vp_buffer, v_bufPos));
}

int setAsduInt(int v_intValue, char* vp_buff, int v_bufPos)
{
  vp_buff[v_bufPos++] = (char)(((unsigned int)v_intValue & 0xff000000) >> 24);
  vp_buff[v_bufPos++] = (char)((v_intValue & 0x00ff0000) >> 16);
  vp_buff[v_bufPos++] = (char)((v_intValue & 0x0000ff00) >> 8);
  vp_buff[v_bufPos++] = (char)(v_intValue & 0x000000ff);
  return(v_bufPos);
}

int setSvAsduData(struct s_svAsduNode* vp_asdu, char* vp_buff, int v_bufPos)
{
  v_bufPos = setSvApduLength((char)0x30, vp_asdu->m_length, vp_buff, v_bufPos);
  v_bufPos = setSvTlvString((char)0x80, vp_asdu->m_id, vp_buff, v_bufPos);
  vp_buff[v_bufPos++] = (char)0x82; //  smpCount
  vp_buff[v_bufPos++] = (char)0x02;
  vp_buff[v_bufPos++] = (char)(vp_asdu->m_smpCount / 256);
  vp_buff[v_bufPos++] = (char)(vp_asdu->m_smpCount % 256);
  vp_buff[v_bufPos++] = (char)0x83; //  confrev
  vp_buff[v_bufPos++] = (char)0x04;
  v_bufPos = setAsduInt(vp_asdu->m_confRev, vp_buff, v_bufPos);
  vp_buff[v_bufPos++] = (char)0x85; //  smpsynch
  vp_buff[v_bufPos++] = (char)0x01;
  vp_buff[v_bufPos++] = (char)0x1;
  vp_buff[v_bufPos++] = (char)0x86; //  smprate
  vp_buff[v_bufPos++] = (char)0x02;
  vp_buff[v_bufPos++] = (char)(vp_asdu->m_samplingRate / 256);
  vp_buff[v_bufPos++] = (char)(vp_asdu->m_samplingRate % 256);
  v_bufPos = setSvApduLength((char)0x87, vp_asdu->m_dataLength, vp_buff, v_bufPos);
  for (int t_i = 0; t_i < vp_asdu->m_channelNum; t_i++)
  {
    v_bufPos = setAsduInt((*vp_asdu->mp_data)[vp_asdu->m_txPoint + t_i], vp_buff, v_bufPos);
    v_bufPos = setAsduInt(0x0, vp_buff, v_bufPos);
  }
  vp_asdu->m_txPoint = ((vp_asdu->m_txPoint >= ((vp_asdu->m_samplingNumber -1) * vp_asdu->m_channelNum)) ? 0 : (vp_asdu->m_txPoint + vp_asdu->m_channelNum));
  vp_asdu->m_smpCount = ((vp_asdu->m_smpCount >= (vp_asdu->m_samplingRate - 1)) ? 0 : (vp_asdu->m_smpCount + 1));
  return(v_bufPos);
}

int setSvAsduList(struct s_linkList* vp_asduHead, char* vp_buff, int v_bufPos)
{
  struct s_linkList* tp_asdu = vp_asduHead;
  while (tp_asdu)
  {
    v_bufPos = setSvAsduData((struct s_svAsduNode*)tp_asdu->mp_data, vp_buff, v_bufPos);
    tp_asdu = tp_asdu->mp_next;
  }
  return(v_bufPos);
}

int getAsduNodeLength(struct s_svPublisher* vp_svData, struct s_svAsduNode* tp_node)
{
  int t_len = 0;
  t_len += getTlvValueSize(tp_node->m_id);
  t_len += 2 + 2; //  smpcnt
  t_len += 2 + 4; //  confref
  t_len += 2 + 1; //  smpsynch
  t_len += 2 + 2; //  smprate
  tp_node->m_dataLength = 8 * tp_node->m_channelNum;  //  data
  t_len += 1 + DEF_actulLength(tp_node->m_dataLength);
  t_len += tp_node->m_dataLength;
  return(t_len);
}

int getSvAsdulength(struct s_svPublisher* vp_svData)
{
  struct s_linkList* tp_node = vp_svData->mp_asduHead;
  vp_svData->m_asduLength = 0;
  while (tp_node != NULL)
  {
    ((struct s_svAsduNode*)(tp_node->mp_data))->m_length = 0;
    ((struct s_svAsduNode*)(tp_node->mp_data))->m_length += getAsduNodeLength(vp_svData, tp_node->mp_data);
    vp_svData->m_asduLength += 1 + DEF_actulLength(((struct s_svAsduNode*)(tp_node->mp_data))->m_length);
    vp_svData->m_asduLength += ((struct s_svAsduNode*)(tp_node->mp_data))->m_length;
    tp_node = tp_node->mp_next;
  }
  return(vp_svData->m_asduLength);
}

int svApduCalculate(struct s_svPublisher* vp_svData)
{
  int t_svApduLength = 0;
  t_svApduLength += 2 + (DEF_actulLength(vp_svData->m_numAsdu));
  vp_svData->m_asduLength = getSvAsdulength(vp_svData);
  t_svApduLength += vp_svData->m_asduLength;
  t_svApduLength += 1 + DEF_actulLength(vp_svData->m_asduLength);
  //  to be continued...

  int t_pduLen = t_svApduLength + 8;
  t_pduLen += 1 + (DEF_actulLength(t_pduLen));
  vp_svData->mp_buffer[vp_svData->m_lengthField] = (char)(t_pduLen / 256);
  vp_svData->mp_buffer[vp_svData->m_lengthField + 1] = (char)(t_pduLen % 256);
  return(t_svApduLength);
}

int svPduEncode(struct s_svPublisher* vp_svData, int v_svApduLength)
{
  int t_bufPos = 0;
  char* t_buff = vp_svData->mp_buffer + vp_svData->m_payloadStart;
  t_bufPos = setSvApduLength((char)0x60, v_svApduLength, t_buff, t_bufPos);
  t_bufPos = setSvApduTlvInt((char)0x80, vp_svData->m_numAsdu, t_buff, t_bufPos);
  t_bufPos = setSvApduLength((char)0xa2, vp_svData->m_asduLength, t_buff, t_bufPos);
  t_bufPos = setSvAsduList(vp_svData->mp_asduHead, t_buff, t_bufPos);
  //  to be continued...

  return(t_bufPos);
}

int svPayloadCreate(struct s_svPublisher* vp_svData)
{
  svHeadCreate(vp_svData);
  int t_svApduLength = svApduCalculate(vp_svData);
  int t_bufPos = svPduEncode(vp_svData, t_svApduLength);
  vp_svData->m_length = vp_svData->m_payloadStart + t_bufPos;
  return(vp_svData->m_length);
}


void svFrameSend(struct s_gooseAndSvThreadData* vp_svThreadData, struct s_svPublisher* vp_svPub)
{
  if (((*vp_svThreadData->mp_timerCount) > (vp_svPub->m_lastTimerCount + vp_svPub->m_frameInterval)) || (vp_svPub->m_lastTimerCount == 0))
  {
    if (svPayloadCreate(vp_svPub) > DEF_maxFrameLen)
    {
      perror("The sv frame length big than DEF_maxFrameLen");
    }
    else
    {
      vp_svPub->m_lastTimerCount = *vp_svThreadData->mp_timerCount;
      sendData(vp_svThreadData->mp_socket, vp_svPub->mp_buffer, vp_svPub->m_length);
    }
    if (vp_svPub->m_lastTimerCount == 0)
    {
      sleep(1);
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
      svFrameSend(vp_svThreadData, tp_svPub);
    }
    //sleep(0);
  }
  svPubDestory(tp_svPub);
}
