#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <iostream>
#include <cstdlib>
#include "ctrlsep.h"
#include <iostream>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include "separ.h"

#include<signal.h>
#include<sys/time.h> 

//#define NO_DEVISES

using namespace std;
/*
void timer_handler(int signum)
{ 
 fputc('x',stderr);
}*/



int main(int argc, char *argv[])
{
  
  /*
  struct sigaction sa;
  struct itimerval timer;
  memset(&sa,0,sizeof(sa));
  sa.sa_handler=&timer_handler;
  sigaction(SIGALRM,&sa,NULL);
  timer.it_value.tv_sec=0;
  timer.it_value.tv_usec=250000;
  timer.it_interval.tv_sec=0;
  timer.it_interval.tv_usec=3000;
  setitimer(ITIMER_REAL,&timer,NULL);*/

  //Separ mys;
  // int listener=socket(AF_INET,SOCK_STREAM,0);

  CtrlSep MyCtrlSep;
  cout << "Hello, world!"<< endl;
  MyCtrlSep.RunSep();
  while(1);
  return EXIT_SUCCESS;
}
