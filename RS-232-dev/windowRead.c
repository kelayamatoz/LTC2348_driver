#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>
#include <signal.h>
#include "rs232.h"
#include "LTC_translate.h"

// TODO: need to use a window size of 24+1 later
#define WIN_SIZE 25
#define POLL_SIZE 1

struct timeval start, end;
long utime, seconds, useconds;
double readNums;

void exitHandler(int);

int main()
{
  signal(SIGINT, exitHandler);
  int n,
  // TODO: check if readBytes can affect performance
  readBytes = 1,
  cport_nr = 0, // /dev/ttys0 is reserved for the testing port
  bdrate = 230400;

  uint8_t winBuf[WIN_SIZE+1];
  memset(winBuf, 0, WIN_SIZE);
  
  char mode[]={'8','N','1',0};
  if(RS232_OpenComport(cport_nr, bdrate, mode))
  {
    printf("Can not open comport\n");
    return(0);
  }

  gettimeofday(&start, NULL);

  uint8_t ch = 0;
  uint8_t lch = 0; // last ch
  int winPtr = 0;
  do
  {
    // it is fair to read one byte each time. Serial port updates way slower than the while loop
    uint8_t rdCh = 0;
    n = RS232_PollComport(cport_nr, &rdCh, readBytes); 
    if (n != 0)
    {
      readNums ++;
      lch = ch;
      ch = rdCh;
      if (lch == 59) //;
      {
//        int readSize = strlen((char*)winBuf);
//        if (readSize != WIN_SIZE)
//        {
//          printf("read a corrupted data seg, read %d bytes\n", readSize);
//        }
//        else
//        {
//          printf("read one data seg = %s\n", winBuf); // indicator for a successful read
//        }

        memset(winBuf, 0, WIN_SIZE);
        winPtr = 0;
      }
      else
      {
        winPtr++;
      }
      
      winBuf[winPtr] = ch;
    }
  }
  while (1); 
}

void exitHandler(int dum)
{
  printf("exiting...\n");
  gettimeofday(&end, NULL);
  seconds = end.tv_sec - start.tv_sec;
  useconds = end.tv_usec - start.tv_usec;
  utime = seconds * 1000000 + useconds;

  double averU = utime / readNums;
  printf("Average read takes %f microseconds. Read in %f entries \n", averU, readNums);

  exit(0);
}
