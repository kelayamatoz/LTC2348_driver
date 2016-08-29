#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "rs232.h"
#include <sys/time.h>

int main()
{
  int n,
      cport_nr=0,        /* /dev/ttyS0 is reserved for the testing port */
      bdrate=230400; 

  uint8_t buf[8192];
  uint8_t compBuf[8192];
  memset(compBuf, 0, 8192);
  memset(buf, 0, 8192);
  char mode[]={'8','N','1',0};

  if(RS232_OpenComport(cport_nr, bdrate, mode))
  {
    printf("Can not open comport\n");
    return(0);
  }

  int lastTracker = 0;
  double readNums = 0;
  struct timeval start, end;
  long utime, seconds, useconds;
  gettimeofday(&start, NULL);

  // remove the first read to clean up buffer
  RS232_PollComport(cport_nr, buf, 4095);
  do
  {
    n = RS232_PollComport(cport_nr, buf, 4095);
    readNums ++;
    if (n + lastTracker >= 8191)
    {
      compBuf[8192] = 0;
      printf("Saturated the buffer; exiting\n");
      printf("read compBuf content = %s", (char*) compBuf);
      break;
    }
    else
    {
      memcpy(compBuf+lastTracker, buf, n);
      lastTracker += n;
    }
  }
  while (1);
  
  gettimeofday(&end, NULL);
  seconds = end.tv_sec - start.tv_sec;
  useconds = end.tv_usec - start.tv_usec;
  utime = seconds * 1000000 + useconds; 

  double averU = utime / readNums;
  printf("Average read takes %f microseconds.\n", averU);
  return(0);
}

