/*
*  goosepub.c
*  by pioevh@163.com 20170320
*/

//include files

#include "goosepub.h"

//  function

void goosePublisherSetEnable(struct s_goosePublisher* vp_gooseData, char* vp_goID, int v_length)
{
  vp_gooseData->m_enable = v_length;
}

void goosePublisherSetGoID(struct s_goosePublisher* vp_gooseData, char* vp_goID, int v_length)
{
  vp_gooseData->m_goID = copyString(vp_goID);
}

void goosePublisherSetGoCbRef(struct s_goosePublisher* vp_gooseData, char* vp_goCbRef, int v_length)
{
  vp_gooseData->m_goCBRef = copyString(vp_goCbRef);
}

void goosePublisherSetDataSetRef(struct s_goosePublisher* vp_gooseData, char* vp_dataSetRef, int v_length)
{
  vp_gooseData->m_dataSetRef = copyString(vp_dataSetRef);
}

void stNumMod(struct s_goosePublisher* vp_gooseData)
{
  vp_gooseData->m_stNum = (vp_gooseData->m_stNum == 0xFFFFFFFF) ? 1 : vp_gooseData->m_stNum + 1;
  vp_gooseData->m_sqNum = 0;
  getDateTime(&vp_gooseData->m_dateTime);
}

void sqNumMod(struct s_goosePublisher* vp_gooseData)
{
  vp_gooseData->m_sqNum = (vp_gooseData->m_sqNum == 0xFFFFFFFF) ? 1 : vp_gooseData->m_sqNum + 1;
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
  vp_gooseData->m_lastTimerCount = 0;
  vp_gooseData->m_enable = 0;
  vp_gooseData->m_length = 0;
  vp_gooseData->m_stNum = 0;
  vp_gooseData->m_sqNum = 0;
  vp_gooseData->m_confRev = 1;
  vp_gooseData->m_test = 0;
  vp_gooseData->m_needsCommission = 0;
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
  int t_bufPos = 12;
  memcpy(vp_gooseData->mp_buffer, vp_gooseData->m_dAddr, DEF_macAddrLen);
  memcpy(vp_gooseData->mp_buffer + 6, vp_gooseData->m_sAddr, DEF_macAddrLen);
  /* Priority tag - IEEE 802.1Q */
  vp_gooseData->mp_buffer[t_bufPos++] = (char)0x81;
  vp_gooseData->mp_buffer[t_bufPos++] = (char)0x00;
  vp_gooseData->mp_buffer[t_bufPos++] = (char)((vp_gooseData->m_priority << 5) + (vp_gooseData->m_vlanId / 256)); /* Priority + VLAN-ID */
  vp_gooseData->mp_buffer[t_bufPos++] = (char)(vp_gooseData->m_vlanId % 256); /* VLAN-ID */
  vp_gooseData->mp_buffer[t_bufPos++] = (char)0x88; /* EtherType GOOSE */
  vp_gooseData->mp_buffer[t_bufPos++] = (char)0xB8;
  vp_gooseData->mp_buffer[t_bufPos++] = DEF_gooseDefaultAppid / 256;
  vp_gooseData->mp_buffer[t_bufPos++] = DEF_gooseDefaultAppid % 256;
  vp_gooseData->m_lengthField = t_bufPos;
  vp_gooseData->mp_buffer[t_bufPos++] = (char)0x00;
  vp_gooseData->mp_buffer[t_bufPos++] = (char)0x00;
  vp_gooseData->mp_buffer[t_bufPos++] = (char)0x00;
  vp_gooseData->mp_buffer[t_bufPos++] = (char)0x00;
  vp_gooseData->mp_buffer[t_bufPos++] = (char)0x00;
  vp_gooseData->mp_buffer[t_bufPos++] = (char)0x00;
  vp_gooseData->m_payloadStart = t_bufPos;
  //  step 1: pdu length calculate
  int t_goosePduLength = 0;
  t_goosePduLength += getTlvValueSize(vp_gooseData->m_goCBRef);
  t_goosePduLength += getTlvValueSize(vp_gooseData->m_dataSetRef);
  if (vp_gooseData->m_goID != NULL)
  {
    t_goosePduLength += getTlvValueSize(vp_gooseData->m_goID);
  }
  else
  {
    vp_gooseData->m_goID = vp_gooseData->m_goCBRef;
    t_goosePduLength += getTlvValueSize(vp_gooseData->m_goCBRef);
  }
  t_goosePduLength += 2 + DEF_actulLength(vp_gooseData->m_timeAllowedToLive);
  t_goosePduLength += 2 + 4 + 4; /* for datetime (UTCTIME) */
  t_goosePduLength += 2 + DEF_actulLength(vp_gooseData->m_stNum);
  t_goosePduLength += 2 + DEF_actulLength(vp_gooseData->m_sqNum);
  t_goosePduLength += 2 + DEF_actulLength(vp_gooseData->m_test);
  t_goosePduLength += 2 + DEF_actulLength(vp_gooseData->m_confRev);
  t_goosePduLength += 2 + DEF_actulLength(vp_gooseData->m_needsCommission);
  //  to be do...
  vp_gooseData->mp_buffer[vp_gooseData->m_lengthField] = (char)((t_goosePduLength + 8) / 256);
  vp_gooseData->mp_buffer[vp_gooseData->m_lengthField + 1] = (char)((t_goosePduLength + 8) % 256);
  //  step 2: pdu encode
  /* Encode GOOSE PDU */
  int t_gooseApduLength = t_goosePduLength - 8;
  t_bufPos = 0;
  char* t_buff = vp_gooseData->mp_buffer+vp_gooseData->m_payloadStart;
  t_bufPos = setGooseApduLength((char)0x61, t_gooseApduLength, t_buff, t_bufPos);
  /* Encode gocbRef */
  t_bufPos = setGooseTlvString((char)0x80, vp_gooseData->m_goCBRef, t_buff, t_bufPos);
  /* Encode timeAllowedToLive */
  t_bufPos = setGooseTlvInt((char)0x81, vp_gooseData->m_timeAllowedToLive, t_buff, t_bufPos);
  /* Encode datSet reference */
  t_bufPos = setGooseTlvString((char)0x82, vp_gooseData->m_dataSetRef, t_buff, t_bufPos);
  /* Encode goID */
  t_bufPos = setGooseTlvString((char)0x83, vp_gooseData->m_goID, t_buff, t_bufPos);
  /* Encode t */
  t_bufPos = setGooseTlvOctet((char)0x84, (char*)&vp_gooseData->m_dateTime, 8, t_buff, t_bufPos);
  /* Encode stNum */
  t_bufPos = setGooseTlvInt((char)0x85, vp_gooseData->m_stNum, t_buff, t_bufPos);
  /* Encode sqNum */
  t_bufPos = setGooseTlvInt((char)0x86, vp_gooseData->m_sqNum, t_buff, t_bufPos);
  /* Encode test */
  t_bufPos = setGooseTlvBoolean((char)0x87, vp_gooseData->m_test, t_buff, t_bufPos);
  /* Encode confRef */
  t_bufPos = setGooseTlvInt((char)0x88, vp_gooseData->m_confRev, t_buff, t_bufPos);
  /* Encode ndsCom */
  t_bufPos = setGooseTlvBoolean((char)0x89, vp_gooseData->m_needsCommission, t_buff, t_bufPos);
  /* Encode numDatSetEntries */

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
  int t_bytesToDelete = t_bytePosition - vp_integer;
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
  char* t_valueArray = (char*)&v_value;
  char t_valueBuffer[5];
  t_valueBuffer[0] = 0;
  int t_i = 0;
  for (t_i = 0; t_i < 4; t_i++)
  {
    t_valueBuffer[t_i + 1] = t_valueArray[t_i];
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
  vp_buffer[v_bufPos++] = (char)((v_value!=0x0) ? 0xff : 0x00);
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

void gooseCmdReg(void** vp_gooseDataModify)
{
  for (int t_i = 0; t_i < DEF_gooseCmdType; t_i++)
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
  void(*t_gooseDataModify[DEF_gooseCmdType])(struct s_goosePublisher*, void*, int);
  gooseCmdReg((void**)(&t_gooseDataModify));
  while (vp_gooseThreadData->m_running)
  {
    if (vp_gooseThreadData->m_cmd > 0 && t_gooseDataModify[vp_gooseThreadData->m_cmd] != NULL)
    {
      t_gooseDataModify[vp_gooseThreadData->m_cmd](t_goosePub, vp_gooseThreadData->mp_value, vp_gooseThreadData->m_length);
      t_goosePub->m_length = 0;
      vp_gooseThreadData->m_cmd = 0;
    }
    if (t_goosePub->m_enable)
    {
      sqNumMod(t_goosePub);
      if (t_goosePub->m_length == 0)
      {
        stNumMod(t_goosePub);
        goosePayloadCreate(t_goosePub);
      }
      if (*(vp_gooseThreadData->mp_timerCount) > (t_goosePub->m_lastTimerCount + t_goosePub->m_frameInterval))
      {
        t_goosePub->m_lastTimerCount = *(vp_gooseThreadData->mp_timerCount);
        sendData(vp_gooseThreadData->mp_socket, t_goosePub->mp_buffer, t_goosePub->m_length);
      }
    }
    //sleep(0);
  }
  goosePubDestory(t_goosePub);
}