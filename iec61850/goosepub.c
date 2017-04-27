/*
*  goosepub.c
*  by pioevh@163.com 20170320
*/

//include files

#include "goosepub.h"

//  function

void gooseCmdReg(void** vp_gooseDataModify)
{
  for (int t_i = 0; t_i < DEF_gooseCmdType; t_i++)
  {
    vp_gooseDataModify[t_i] = NULL;
  }
  vp_gooseDataModify[1] = goosePublisherSetEnable;
  vp_gooseDataModify[101] = goosePublisherSetAppid;
  vp_gooseDataModify[102] = goosePublisherSetVlanId;
  vp_gooseDataModify[103] = goosePublisherSetGoCbRef;
  vp_gooseDataModify[104] = goosePublisherSetGoID;
  vp_gooseDataModify[105] = goosePublisherSetDataSetRef;
  vp_gooseDataModify[106] = goosePublisherSetDataSetEntriesInt;
  vp_gooseDataModify[107] = goosePublisherSetMac;
  vp_gooseDataModify[108] = goosePublisherSetVlanPriority;
}

void goosePublisherSetMac(struct s_goosePublisher* vp_gooseData, char* vp_buffer, int v_length)
{
  memcpy(vp_gooseData->m_dAddr, vp_buffer, DEF_macAddrLen);
}

void goosePublisherSetVlanPriority(struct s_goosePublisher* vp_gooseData, char* vp_buffer, int v_length)
{
  vp_gooseData->m_priority = (char)(v_length & 0x7);
}

void goosePublisherSetEnable(struct s_goosePublisher* vp_gooseData, char* vp_buffer, int v_length)
{
  vp_gooseData->m_enable = v_length;
}

void goosePublisherSetAppid(struct s_goosePublisher* vp_gooseData, char* vp_buffer, int v_length)
{
  vp_gooseData->m_appId = (uint16_t)(v_length & 0xffff);
}

void goosePublisherSetVlanId(struct s_goosePublisher* vp_gooseData, char* vp_buffer, int v_length)
{
  vp_gooseData->m_vlanId = (uint16_t)(v_length & 0xffff);
}

void goosePublisherSetGoID(struct s_goosePublisher* vp_gooseData, char* vp_buffer, int v_length)
{
  vp_gooseData->m_goID = copyString(vp_buffer);
}

void goosePublisherSetGoCbRef(struct s_goosePublisher* vp_gooseData, char* vp_buffer, int v_length)
{
  vp_gooseData->m_goCBRef = copyString(vp_buffer);
}

void goosePublisherSetDataSetRef(struct s_goosePublisher* vp_gooseData, char* vp_buffer, int v_length)
{
  vp_gooseData->m_dataSetRef = copyString(vp_buffer);
}

void goosePublisherSetDataSetEntriesInt(struct s_goosePublisher* vp_gooseData, char* vp_dataSetRef, int v_length)
{
  struct s_dataSetNode* tp_node = (struct s_dataSetNode*)malloc(sizeof(struct s_dataSetNode));
  int* tp_i = (int*)malloc(sizeof(int));
  *tp_i = (int)(DEF_modIntEndian((unsigned int)v_length));
  tp_node->m_data = tp_i;
  tp_node->m_type = (unsigned char)DEF_dataSetTypeInteger;
  tp_node->m_length = DEF_actulLength(v_length);
  struct s_linkList* tp_linkList = linkListCreate(tp_node);
  linkListAppend(&vp_gooseData->mp_dataSetHead, tp_linkList);
  vp_gooseData->m_numDataSetEntries++;
}

void stNumMod(struct s_goosePublisher* vp_gooseData)
{
  vp_gooseData->m_stNum = ((vp_gooseData->m_stNum == 0xFFFFFFFF) ? 1 : vp_gooseData->m_stNum + 1);
  vp_gooseData->m_sqNum = 0;
  getDateTime(&vp_gooseData->m_dateTime);
  setDataTimeToUtc(&vp_gooseData->m_dateTime);
}

void sqNumMod(struct s_goosePublisher* vp_gooseData)
{
  vp_gooseData->m_sqNum = ((vp_gooseData->m_sqNum == 0xFFFFFFFF) ? 1 : vp_gooseData->m_sqNum + 1);
}

void gooseBufferPrepare(struct s_goosePublisher* vp_gooseData, struct s_gooseAndSvThreadData* vp_gooseThreadData)
{
  char t_mac[DEF_macAddrLen] = DEF_gooseDefaultDMac;
  memcpy(vp_gooseData->m_dAddr, t_mac, DEF_macAddrLen);
  memcpy(vp_gooseData->m_sAddr, vp_gooseThreadData->mp_socket->m_interfaceMacAddr, DEF_macAddrLen);
  vp_gooseData->m_priority = DEF_gooseDefaultPriority;
  vp_gooseData->m_vlanId = DEF_gooseDefaultVlanId;
  vp_gooseData->m_appId = DEF_gooseDefaultAppid;
  vp_gooseData->m_timeAllowedToLive = DEF_gooseTimeAllowedToLive;
  vp_gooseData->mp_buffer = (char*)malloc(DEF_maxFrameLen);
  memset(vp_gooseData->mp_buffer, 0, DEF_maxFrameLen);
  vp_gooseData->m_frameInterval = DEF_gooseDefaultFrameInterval;
  vp_gooseData->m_confRev = 1;
}

struct s_goosePublisher* goosePubCreate(struct s_gooseAndSvThreadData* vp_gooseThreadData)
{
  struct s_goosePublisher* tp_gooseData = (struct s_goosePublisher*)malloc(sizeof(struct s_goosePublisher));
  memset(tp_gooseData, 0, sizeof(struct s_goosePublisher));
  gooseBufferPrepare(tp_gooseData, (struct s_gooseAndSvThreadData*)vp_gooseThreadData);
  return(tp_gooseData);
}

void gooseDataSetEntriesDestory(struct s_linkList* vp_dataSetHead)
{
  struct s_linkList* tp_node = vp_dataSetHead;
  struct s_linkList* tp_waitFree = NULL;
  while (tp_node != NULL)
  {
    tp_waitFree = tp_node;
    tp_node = tp_node->mp_next;
    free(tp_waitFree->mp_data);
    free(tp_waitFree);
  }
}

void goosePubDestory(struct s_goosePublisher* vp_gooseData)
{
  (vp_gooseData->m_goID == NULL) ? vp_gooseData->m_goID = NULL : free(vp_gooseData->m_goID);
  (vp_gooseData->m_goCBRef == NULL) ? vp_gooseData->m_goCBRef = NULL : free(vp_gooseData->m_goCBRef);
  (vp_gooseData->m_dataSetRef == NULL) ? vp_gooseData->m_dataSetRef = NULL : free(vp_gooseData->m_dataSetRef);
  gooseDataSetEntriesDestory(vp_gooseData->mp_dataSetHead);
  printf("service appid#%-4d stop\n", vp_gooseData->m_appId);
  free(vp_gooseData->mp_buffer);
  free(vp_gooseData);
}

int gooseHeadCreate(struct s_goosePublisher* vp_gooseData)
{
  int t_bufPos = 12;
  memcpy(vp_gooseData->mp_buffer, vp_gooseData->m_dAddr, DEF_macAddrLen);
  memcpy(vp_gooseData->mp_buffer + 6, vp_gooseData->m_sAddr, DEF_macAddrLen);
  /* Priority tag - IEEE 802.1Q */
  vp_gooseData->mp_buffer[t_bufPos++] = (char)0x81;
  vp_gooseData->mp_buffer[t_bufPos++] = (char)0x00;
  //vp_gooseData->m_vlanId = 1;
  vp_gooseData->mp_buffer[t_bufPos++] = (char)((vp_gooseData->m_priority << 5) + (vp_gooseData->m_vlanId / 256)); /* Priority + VLAN-ID */
  vp_gooseData->mp_buffer[t_bufPos++] = (char)(vp_gooseData->m_vlanId % 256); /* VLAN-ID */
  vp_gooseData->mp_buffer[t_bufPos++] = (char)0x88; /* EtherType GOOSE */
  vp_gooseData->mp_buffer[t_bufPos++] = (char)0xB8;
  vp_gooseData->mp_buffer[t_bufPos++] = (char)(vp_gooseData->m_appId / 256);
  vp_gooseData->mp_buffer[t_bufPos++] = (char)(vp_gooseData->m_appId % 256);
  vp_gooseData->m_lengthField = t_bufPos;
  vp_gooseData->mp_buffer[t_bufPos++] = (char)0x00; //  PDU length
  vp_gooseData->mp_buffer[t_bufPos++] = (char)0x00;
  vp_gooseData->mp_buffer[t_bufPos++] = (char)0x00; //  PDU reserve
  vp_gooseData->mp_buffer[t_bufPos++] = (char)0x00;
  vp_gooseData->mp_buffer[t_bufPos++] = (char)0x00;
  vp_gooseData->mp_buffer[t_bufPos++] = (char)0x00;
  vp_gooseData->m_payloadStart = t_bufPos;  //  APDU pos
  return(t_bufPos);
}

int getGooseAllDataLength(struct s_linkList* vp_dataSetHead)
{
  int t_len = 0;
  struct s_linkList* tp_node = vp_dataSetHead;
  struct s_dataSetNode* tp_data = NULL;
  while (tp_node != NULL)
  {
    tp_data = tp_node->mp_data;
    switch ((unsigned int)tp_data->m_type)
    {
    case DEF_dataSetTypeInteger:
      t_len += 2 + (DEF_actulLength(*((int*)tp_data->m_data)));
      break;
    default:
      break;
    }
    tp_node = tp_node->mp_next;
  }
  return(t_len);
}

int gooseApduCalculate(struct s_goosePublisher* vp_gooseData)
{
  int t_gooseApduLength = 0;
  t_gooseApduLength += getTlvValueSize(vp_gooseData->m_goCBRef);
  t_gooseApduLength += getTlvValueSize(vp_gooseData->m_dataSetRef);
  if (vp_gooseData->m_goID != NULL)
  {
    t_gooseApduLength += getTlvValueSize(vp_gooseData->m_goID);
  }
  else
  {
    vp_gooseData->m_goID = vp_gooseData->m_goCBRef;
    t_gooseApduLength += getTlvValueSize(vp_gooseData->m_goCBRef);
  }
  t_gooseApduLength += 2 + (DEF_actulLength(vp_gooseData->m_timeAllowedToLive));
  t_gooseApduLength += 2 + 4 + 4; /* for datetime (UTCTIME) */
  t_gooseApduLength += 2 + (DEF_actulLength(vp_gooseData->m_stNum));
  t_gooseApduLength += 2 + (DEF_actulLength(vp_gooseData->m_sqNum));
  t_gooseApduLength += 2 + (DEF_actulLength(vp_gooseData->m_test));
  t_gooseApduLength += 2 + (DEF_actulLength(vp_gooseData->m_confRev));
  t_gooseApduLength += 2 + (DEF_actulLength(vp_gooseData->m_needsCommission));
  t_gooseApduLength += 2 + (DEF_actulLength(vp_gooseData->m_numDataSetEntries));
  vp_gooseData->m_dataSetLength = getGooseAllDataLength(vp_gooseData->mp_dataSetHead);
  t_gooseApduLength += 1 + (DEF_actulLength(vp_gooseData->m_dataSetLength)) + DEF_padLengthHead(vp_gooseData->m_dataSetLength);
  t_gooseApduLength += vp_gooseData->m_dataSetLength;
  //  to be continued...

  int t_pduLen = t_gooseApduLength + 8;
  t_pduLen += 1 + (DEF_actulLength(t_gooseApduLength));
  vp_gooseData->mp_buffer[vp_gooseData->m_lengthField] = (char)(t_pduLen / 256);
  vp_gooseData->mp_buffer[vp_gooseData->m_lengthField + 1] = (char)(t_pduLen % 256);
  return(t_gooseApduLength);
}

int goosePduEncode(struct s_goosePublisher* vp_gooseData, int v_gooseApduLength)
{
  int t_bufPos = 0;
  char* t_buff = vp_gooseData->mp_buffer + vp_gooseData->m_payloadStart;
  t_bufPos = setGooseApduLength((char)0x61, v_gooseApduLength, t_buff, t_bufPos);
  t_bufPos = setGooseTlvString((char)0x80, vp_gooseData->m_goCBRef, t_buff, t_bufPos);
  t_bufPos = setGooseTlvInt((char)0x81, vp_gooseData->m_timeAllowedToLive, t_buff, t_bufPos); //  endian
  t_bufPos = setGooseTlvString((char)0x82, vp_gooseData->m_dataSetRef, t_buff, t_bufPos);
  t_bufPos = setGooseTlvString((char)0x83, vp_gooseData->m_goID, t_buff, t_bufPos);
  t_bufPos = setGooseTlvOctet((char)0x84, (char*)&vp_gooseData->m_dateTime, 8, t_buff, t_bufPos); //  endian
  t_bufPos = setGooseTlvInt((char)0x85, vp_gooseData->m_stNum, t_buff, t_bufPos);  //  endian
  vp_gooseData->m_sqNumPos = vp_gooseData->m_payloadStart + t_bufPos;
  t_bufPos = setGooseTlvInt((char)0x86, vp_gooseData->m_sqNum, t_buff, t_bufPos);  //  endian
  t_bufPos = setGooseTlvBoolean((char)0x87, vp_gooseData->m_test, t_buff, t_bufPos);
  t_bufPos = setGooseTlvInt((char)0x88, vp_gooseData->m_confRev, t_buff, t_bufPos);  //  endian
  t_bufPos = setGooseTlvBoolean((char)0x89, vp_gooseData->m_needsCommission, t_buff, t_bufPos);
  t_bufPos = setGooseTlvInt((char)0x8a, vp_gooseData->m_numDataSetEntries, t_buff, t_bufPos);  //  endian
  t_bufPos = setGooseDataSetLength((char)0xab, vp_gooseData->m_dataSetLength, t_buff, t_bufPos);  //  endian
  t_bufPos = setGooseDataSetList(vp_gooseData->mp_dataSetHead, t_buff, t_bufPos);
  //  to be continued...

  return(t_bufPos);
}

int setGooseDataSetLength(char v_tag, int v_length, char* vp_buffer, int v_bufPos)
{
  return(setGooseApduLength(v_tag, v_length, vp_buffer, v_bufPos));
}

int setGooseDataSetList(struct s_linkList* vp_dataSetHead, char* vp_buffer, int v_bufPos)
{
  struct s_linkList* tp_node = vp_dataSetHead;
  struct s_dataSetNode* tp_data = NULL;
  while (tp_node != NULL)
  {
    tp_data = tp_node->mp_data;
    switch ((unsigned int)tp_data->m_type)
    {
    case DEF_dataSetTypeInteger:
      v_bufPos = setGooseTlvInt((char)DEF_dataSetTypeInteger, *((unsigned int*)tp_data->m_data), vp_buffer, v_bufPos);
      break;
    default:
      break;
    }
    tp_node = tp_node->mp_next;
  }
  return(v_bufPos);
}

int goosePayloadCreate(struct s_goosePublisher* vp_gooseData)
{
  gooseHeadCreate(vp_gooseData);
  int t_gooseApduLength = gooseApduCalculate(vp_gooseData);
  int t_bufPos = goosePduEncode(vp_gooseData, t_gooseApduLength);
  vp_gooseData->m_length = vp_gooseData->m_payloadStart + t_bufPos;
  return(vp_gooseData->m_length);
}

int setGooseTlvLengthValue(int v_length, char* vp_buffer, int v_bufPos)
{
  if (v_length < 128)
  {
    vp_buffer[v_bufPos++] = (char)v_length;
  }
  else if (v_length < 256)
  {
    vp_buffer[v_bufPos++] = (char)0x81;
    vp_buffer[v_bufPos++] = (char)v_length;
  }
  else
  {
    vp_buffer[v_bufPos++] = (char)0x82;
    vp_buffer[v_bufPos++] = (char)(v_length / 256);
    vp_buffer[v_bufPos++] = (char)(v_length % 256);
  }
  return(v_bufPos);
}

int setGooseApduLength(char v_tag, int v_length, char* vp_buffer, int v_bufPos)
{
  vp_buffer[v_bufPos++] = v_tag;
  v_bufPos = setGooseTlvLengthValue(v_length, vp_buffer, v_bufPos);
  return(v_bufPos);
}

int setGooseTlvString(char v_tag, char* vp_string, char* vp_buffer, int v_bufPos)
{
  vp_buffer[v_bufPos++] = v_tag;
  if (vp_string != NULL)
  {
    int t_length = (int)strlen(vp_string);
    v_bufPos = setGooseTlvLengthValue(t_length, vp_buffer, v_bufPos);
    for (int t_i = 0; t_i < t_length; t_i++)
    {
      vp_buffer[v_bufPos++] = vp_string[t_i];
    }
  }
  else
  {
    vp_buffer[v_bufPos++] = 0;
  }
  return(v_bufPos);
}

int getTlvIntArrayLength(char* vp_integer, int v_originalSize)
{
  char* t_integerEnd = vp_integer + v_originalSize - 1;
  char* t_bytePosition;
  for (t_bytePosition = vp_integer; t_bytePosition < t_integerEnd; t_bytePosition++)
  {
    if (t_bytePosition[0] == 0x00)
    {
      if ((t_bytePosition[1] & 0x80) == 0)
        continue;
    }
    else if (t_bytePosition[0] == 0xff)
    {
      if (t_bytePosition[1] & 0x80)
        continue;
    }
    break;
  }
  int t_bytesToDelete = (int)(t_bytePosition - vp_integer);
  int t_newSize = v_originalSize;
  if (t_bytesToDelete)
  {
    t_newSize -= t_bytesToDelete;
    char* t_newEnd = vp_integer + t_newSize;
    char* t_newBytePosition;
    for (t_newBytePosition = vp_integer; t_newBytePosition < t_newEnd; t_newBytePosition++)
    {
      *t_newBytePosition = *t_bytePosition;
      t_bytePosition++;
    }
  }
  return(t_newSize);
}

int setGooseTlvInt(char v_tag, uint32_t v_value, char* vp_buffer, int v_bufPos)
{
  int t_i = 0;
  char* t_valueArray = (char*)(&v_value);
  char t_valueBuffer[sizeof(int) + 1] = { 0 };
  for (t_i = 0; t_i < sizeof(int); t_i++)
  {
    //t_valueBuffer[t_i + 1] = *(t_valueArray + t_i); //  endian
    t_valueBuffer[t_i + 1] = t_valueArray[(int)sizeof(int) - t_i - 1]; //  endian
  }
  int t_size = getTlvIntArrayLength(t_valueBuffer, 5);
  vp_buffer[v_bufPos++] = v_tag;
  vp_buffer[v_bufPos++] = (char)t_size;
  for (t_i = 0; t_i < t_size; t_i++)
  {
    vp_buffer[v_bufPos++] = t_valueBuffer[t_i];
  }
  return(v_bufPos);
}

int setGooseTlvBoolean(char v_tag, int v_value, char* vp_buffer, int v_bufPos)
{
  vp_buffer[v_bufPos++] = v_tag;
  vp_buffer[v_bufPos++] = 1;
  vp_buffer[v_bufPos++] = (char)((v_value != 0x0) ? 0xff : 0x00);
  return(v_bufPos);
}

int setGooseTlvOctet(char v_tag, char* vp_octet, int t_octetSize, char* vp_buffer, int v_bufPos)
{
  vp_buffer[v_bufPos++] = v_tag;
  v_bufPos = setGooseTlvLengthValue(t_octetSize, vp_buffer, v_bufPos);
  int t_i = 0;
  for (t_i = 0; t_i < t_octetSize; t_i++)
  {
    vp_buffer[v_bufPos++] = vp_octet[t_i];
  }
  return(v_bufPos);
}

void gooseDataUpdate(struct s_gooseAndSvThreadData* vp_gooseThreadData, struct s_goosePublisher* vp_goosePub)
{
  if (vp_gooseThreadData->m_cmd > 0 && g_gooseDataModify[vp_gooseThreadData->m_cmd] != NULL)
  {
    (g_gooseDataModify[vp_gooseThreadData->m_cmd])(vp_goosePub, vp_gooseThreadData->mp_value, vp_gooseThreadData->m_length);
    vp_goosePub->m_length = 0;
    vp_gooseThreadData->m_cmd = 0;
  }
}

void gooseFrameSend(struct s_gooseAndSvThreadData* vp_gooseThreadData, struct s_goosePublisher* vp_goosePub)
{
  if (((*vp_gooseThreadData->mp_timerCount) > (vp_goosePub->m_lastTimerCount + vp_goosePub->m_frameInterval)) || (vp_goosePub->m_lastTimerCount == 0))
  {
    vp_goosePub->m_lastTimerCount = *vp_gooseThreadData->mp_timerCount;
    sqNumMod(vp_goosePub);
    goosePayloadCreate(vp_goosePub);
    sendData(vp_gooseThreadData->mp_socket, vp_goosePub->mp_buffer, vp_goosePub->m_length);
    if (vp_goosePub->m_lastTimerCount == 0)
    {
      sleep(1);
    }
  }
}

void gooseThreadRun(struct s_gooseAndSvThreadData* vp_gooseThreadData)
{
  struct s_goosePublisher* tp_goosePub = goosePubCreate(vp_gooseThreadData);
  gooseCmdReg((void**)(&g_gooseDataModify));
  while (*vp_gooseThreadData->m_running)
  {
    gooseDataUpdate(vp_gooseThreadData, tp_goosePub);
    if (tp_goosePub->m_enable)
    {
      if (tp_goosePub->m_length == 0)
      {
        stNumMod(tp_goosePub);
        if (goosePayloadCreate(tp_goosePub) > DEF_maxFrameLen)
        {
          perror("The goose frame length big than DEF_maxFrameLen");
          break;
        }
      }
      gooseFrameSend(vp_gooseThreadData, tp_goosePub);
    }
    //sleep(0);
  }
  goosePubDestory(tp_goosePub);
}