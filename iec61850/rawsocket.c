/*
*  rawsocket.c
*  by pioevh@163.com 20170320
*/

//include files

#include "rawsocket.h"

//  function

void getInterfaceAddress(int v_socket, char* v_interfaceNmae, char* v_interfaceAddr)
{
  struct ifreq t_buffer = { 0 };
  strncpy(t_buffer.ifr_name, v_interfaceNmae, IFNAMSIZ);
  if (ioctl(v_socket, SIOCGIFHWADDR, &t_buffer) == -1)
  {
    perror("Failed to get interface address -> exit");
    exit(1);
  }
  memcpy(v_interfaceAddr, t_buffer.ifr_hwaddr.sa_data,DEF_macAddrLen);
}

int getInterfaceIndex(int v_socket, char* v_interfaceNmae)
{
  struct ifreq t_ifr;
  strncpy(t_ifr.ifr_name, v_interfaceNmae, IFNAMSIZ);
  if (ioctl(v_socket, SIOCGIFINDEX, &t_ifr) == -1)
  {
    perror("Failed to get interface index -> exit");
    exit(1);
  }
  int interfaceIndex = t_ifr.ifr_ifindex;
  if (ioctl(v_socket, SIOCGIFFLAGS, &t_ifr) == -1)
  {
    perror("Problem getting device flags -> exit");
    exit(1);
  }
  t_ifr.ifr_flags |= IFF_PROMISC;
  if (ioctl(v_socket, SIOCSIFFLAGS, &t_ifr) == -1)
  {
    perror("Setting device to promiscuous mode failed -> exit");
    exit(1);
  }
  return(interfaceIndex);
}

struct s_ethernetSocket* createSocket(char* v_interfaceNmae, char* vp_addr)
{
  struct s_ethernetSocket* t_socket = (struct s_ethernetSocket*)malloc(sizeof(struct s_ethernetSocket));
  memset(t_socket, 0, sizeof(struct s_ethernetSocket));
  t_socket->m_rawSocket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if (t_socket->m_rawSocket == -1)
  {
    perror("Failed to creating raw socket -> exit");
    free(t_socket);
    exit(1);
  }
  t_socket->m_socketAddress.sll_family = PF_PACKET;
  t_socket->m_socketAddress.sll_protocol = htons(ETH_P_IP);
  //t_socket->m_socketAddress.sll_protocol=htons(ETH_P_ALL);
  //t_socket->m_socketAddress.sll_protocol=htons(0x0806);
  getInterfaceAddress(t_socket->m_rawSocket, v_interfaceNmae, t_socket->m_interfaceMacAddr);
  t_socket->m_socketAddress.sll_ifindex = getInterfaceIndex(t_socket->m_rawSocket, v_interfaceNmae);
  t_socket->m_socketAddress.sll_hatype = ARPHRD_ETHER;
  t_socket->m_socketAddress.sll_pkttype = PACKET_OTHERHOST;
  t_socket->m_socketAddress.sll_halen = ETH_ALEN;
  memset(t_socket->m_socketAddress.sll_addr, 0, sizeof(t_socket->m_socketAddress.sll_addr));
  if (vp_addr != NULL)
  {
    memcpy(t_socket->m_socketAddress.sll_addr, vp_addr, DEF_macAddrLen);
  }
  t_socket->m_isBind = 0;
  return(t_socket);
}

int receiveData(struct s_ethernetSocket* vp_socket, char* vp_buffer, int v_length)
{
  if (vp_socket->m_isBind == 0)
  {// raw socket must be binded,else the frame be duplicate
    if (bind(vp_socket->m_rawSocket, (struct sockaddr*)&vp_socket->m_socketAddress, sizeof(vp_socket->m_socketAddress)) == 0)
    {
      vp_socket->m_isBind = 1;
    }
    else
    {
      perror("Failed to bind -> exit");
      exit(1);
    }
  }
  return((int)recvfrom(vp_socket->m_rawSocket, vp_buffer, (unsigned int)v_length, MSG_DONTWAIT, 0, 0));
}

void sendData(struct s_ethernetSocket* vp_socket, char* vp_buffer, int v_length)
{
  sendto(vp_socket->m_rawSocket, vp_buffer, (unsigned int)v_length, 0, (struct sockaddr*)&(vp_socket->m_socketAddress), sizeof(vp_socket->m_socketAddress));
  g_byteCount += v_length;
}

void destorySocket(struct s_ethernetSocket* vp_socket)
{
  close(vp_socket->m_rawSocket);
  free(vp_socket);
}