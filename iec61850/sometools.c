/*
*  sometools.c
*  by pioevh@163.com 20170320
*/

//include files

#include "sometools.h"

//  function

int memDisp(void* vp_buffer, int v_length)
{
  int t_i = 0;
  char* t_ch = (char*)vp_buffer;
  for (t_i = 0; t_i<v_length; t_i++)
  {
    if (t_i % 16 == 0)
    {
      printf("\n");
    }
    else if (t_i % 8 == 0)
    {
      printf("  ");
    }
    printf("%02x ", (*(t_ch + t_i)) & 0xff);
  }
  printf("\n");
  return(0);
}

int createThread(void* vp_function, void* vp_data)
{
  pthread_t t_thread;
  if (pthread_create(&t_thread, NULL, vp_function, vp_data) != 0)
  {
    perror("Failed to create pthread ");
  }
  return(0);
}

void getDateTime(struct timeval* vp_time)
{
  gettimeofday(vp_time, NULL);
}
