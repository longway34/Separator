/***************************************************************************
 *   Copyright (C) 2006 by and   *
 *   and@andrey   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "sdevice.h"
#include <stdio.h>
#include <unistd.h>

static bool f_m50u;
using namespace std;
SDevice::SDevice()
{
 cout << "Hello, SDevice! " << endl;
 for (int i=0;i<3;i++) fm50[i]=false;
 
 sizebuff=0;
 ba=BADDR;
 ba2=BADDR2;
 
 ioperm(ba,0x20,1);
 ioperm(ba2,0x20,1);
 
// afifo=inw(ba) & 0x03ff;
 afifo=inw_("SDevice::SDevice(): ", ba) & 0x03ff;
 fifo=afifo;
 sfw0=0;
 sfw6=0;
// outw(sfw0 | 0x20,ba);
 outw_("SDevice::SDevice(): ",sfw0 | 0x20,ba);
 sfw0=sfw0 | 0x10;
// outw(sfw0,ba);
// outw(sfw0 | 0x40,ba);
 outw_("SDevice::SDevice(): ",sfw0,ba);
 outw_("SDevice::SDevice(): ",sfw0 | 0x40,ba);
 sfw0=sfw0 | 0x0f;
// outw(sfw0,ba);
// outw(1,ba+8);
 outw_("SDevice::SDevice(): ",sfw0,ba);
 outw_("SDevice::SDevice(): ",1,ba+8);
 
 // init ada
 sf0=0;
// outb(sf0,ba2);
 outb_("SDevice::SDevice(): ",sf0,ba2);
 sf1=34;
// outb(sf1,ba2+1);
 outb_("SDevice::SDevice(): ",sf1,ba2+1);
 sf2=0x30;
 sf2&=~16;
 sf2|=64;
 sf2|=128;
// outb(sf2,ba2+2);
 outb_("SDevice::SDevice(): ",sf2,ba2+2);
 sf3=0x20;
// outb(sf3,ba2+3);
 outb_("SDevice::SDevice(): ",sf3,ba2+3);
 
 sf9=0xfd;
// outb(sf9,ba2+9);
 outb_("SDevice::SDevice(): ",sf9,ba2+9);
 sf10=0xfe;
// outb(sf10,ba2+10);
 outb_("SDevice::SDevice(): ",sf10,ba2+10);
 
 initdac();
 for(int i=0;i<4;i++) initdacchan(i);
 for(int i=0;i<4;i++) setdac(i,0);
 /*
 cout<<"adc(4)="<<adc(4)<<endl;
 cout<<"adc(4)="<<adc(4)<<endl;

 cout<<"adc(5)="<<adc(4)<<endl;
 cout<<"adc(5)="<<adc(4)<<endl; */
 //set_lht(10,20);
 setmode(0);
 
 read_tm();
 read_lt();
 read_ht();
 //set_puw(4000);
 /*
 for (int i=0;i<4;i++){
  set_im(i,true);
  sleep(1);
  set_im(i,false);
  sleep(1);
 } 
 on_offsep(true);
 on_offosw(true);
 sleep(3);
 on_offsep(false);
 on_offosw(false);*/
}

SDevice::~SDevice()
{
 for (int i=0;i<3;i++){
  if (fm50[i]==true) close(hm50[i]);
 }
 
}
void SDevice::getbuff()
{
 
 int ik=0;
 do{
//  fifo=inw(ba+2);
  fifo=inw_("SDevice::getbuff(): ", ba+2);
  if(ik++>10) break;
 }
 while((fifo && 0x4000)!=0x4000);
// fifo=inw(ba+2) & 0x03ff;
 fifo=inw_("SDevice::getbuff(): ", ba+2) & 0x03ff;
 //cout << "fifo  " << fifo <<endl;
 int wfifo=fifo-afifo;
 if (wfifo<0) wfifo=wfifo+SizeFifo;
 afifo=fifo;
 for (int i=0;i<wfifo;i++){
//  buff[i]=inw(ba+4);
  buff[i]=inw_("SDevice::getbuff(): ", ba+4);
 }
 sizebuff=wfifo;
 //cout << "fifo  " << fifo <<endl;
}
void SDevice::setudeu()
{
 //cout << " setudeu kod " << kod[0] << endl;

 for (int i=0;i<4;i++) kod[i]<<=6;
 
 sfw6&=!0x10;
 sfw6&=!0x20;
// outw(sfw6,ba+6);
 outw_("SDevice::setudeu(): ", sfw6,ba+6);
 int w=0;
 for(int i=0;i<10;i++){
  for(int j=0;j<4;j++){
   w>>=1;
   if ((kod[j] & 0x8000)==0x8000) w|=0x8000;
   
   kod[j]<<=1;
  }
  
  w>>=12;
  
//  outw(sfw6 | w,ba+6);
//  outw(sfw6 | w | 0x20,ba+6);
//  outw(sfw6 | w,ba+6);
  outw_("SDevice::setudeu(): ", sfw6 | w,ba+6);
  outw_("SDevice::setudeu(): ", sfw6 | w | 0x20,ba+6);
  outw_("SDevice::setudeu(): ", sfw6 | w,ba+6);
 }
 sfw6|=0x10;
// outw(sfw6,ba+6);
 outw_("SDevice::setudeu(): ", sfw6,ba+6);
 //cout << " setudeu kod " << kod[0] << endl;
}
void SDevice::setptdeu()
{
 for (int i=0;i<4;i++) 
   //cout << " setptdeu kod " << kod[i] << endl;
  
  sfw6|=0x10;
  sfw6&=!0x80;
  sfw6&=!0x20;
//  outw(sfw6,ba+6);
  outw_("SDevice::setptdeu(): ", sfw6,ba+6);
  for(int j=0;j<4;j++){ 
    sfw6&=!0x80;
//    outw(sfw6,ba+6);
    outw_("SDevice::setptdeu(): ", sfw6,ba+6);
    int k;
    switch (j){
     case 0:{k=1;break;}
     case 1:{k=3;break;}
     case 2:{k=2;break;}
     case 3:{k=0;break;}
   } 
   int w=0;
   w=k;
   w<<=8;
   w+=kod[j];
   w<<=6;
   for(int i=0;i<10;i++){
    if ((w & 0x8000)==0x8000) sfw6|=0x08;
      else sfw6&=!0x08;
//    outw(sfw6 ,ba+6);
//    outw(sfw6 | 0x20,ba+6);
//    outw(sfw6 ,ba+6);
    outw_("SDevice::setptdeu(): ", sfw6 ,ba+6);
    outw_("SDevice::setptdeu(): ", sfw6 | 0x20,ba+6);
    outw_("SDevice::setptdeu(): ", sfw6 ,ba+6);
    w<<=1;      
   }
  sfw6|=0x80;
//  outw(sfw6,ba+6);
  outw_("SDevice::setptdeu(): ", sfw6,ba+6);
  }
 sfw6|=0x80;
// outw(sfw6,ba+6); 
 outw_("SDevice::setptdeu(): ", sfw6,ba+6); 
}	

int  SDevice::km_m50(int num,int km)
{
 
 char*  st;
 char  bst[10];
 char  buf[10];
 char* s[3];
 int   sz=1;
 bool  fread=false;
 st="z";
 cout <<" KM " <<km<<endl; 
 f_m50u=false;
 switch (km){
  case 0  : st="z";buf[0]='z';on_offexp(false,num); break;
  case 1  : st="x";buf[0]='x';on_offexp(true,num); break;
  case 10 : st="M00";sz=3;s[0]="M";s[1]="0";s[2]="0";break;
  case 11 : st="M01";sz=3;s[0]="M";s[1]="0";s[2]="1";break;
  case 12 : st="M02";sz=3;s[0]="M";s[1]="0";s[2]="2";break;
  case 13 : st="M03";sz=3;s[0]="M";s[1]="0";s[2]="3";break;
  case 14 : st="s";break;
  case 15 : st="m";buf[0]='m';break;
  case 16 : st="t";break;
  case 17 : st="j";break;
  case 18 : st="u";f_m50u=true;break;
  case 19 : st="n";break;
 }
 int t;
 if(km==10 || km==11 || km==12 || km==13)
 {
   
   //for (int i=0;i<30;i++) {
    //t=write(hm50[num],st,3);
    //sleep(1);

    /*for(int j=0;j<3;j++)
    {
        //t=write(hm50[num],(void*)&buf[j],1);
        t=write(hm50[num],s[j],1);
        sleep(1);
        sleep(1);
        sleep(1);
        cout <<num<< " M00 "<<s[j]<<"  "<< endl;
    }*/
    //Alex 24.01.2012
    t = 0;
    for( int j = 0; j < sz + 1; j++ )
    {
        t += write( hm50[num], & st[ j ], 1 );
        usleep( 50 * 1000 );
        read(hm50[num],buf,10);
        //usleep( 50 * 1000 );
    }

  //}
  //int t=read(hm50[num],buf,10);
  //cout << " read="<<t<<endl;
 }
 else
 {
     t=write(hm50[num],st,sz);
 }


 //t=write(hm50[num],(void*)&buf[0],1); 
 cout << "M50 num = " <<num<< " write \""<<st<<"\"  "<< t<< endl;
 /*t=read(hm50[num],buf,10);
 if (fread==true){
    t=read(hm50[num],buf,10);
    cout << " read="<<t<<endl;
    for(int i=0;i<t;i++) cout << "bf=" <<buf[i]<<endl; 
 }*/
 return 0;
}
int SDevice::setm50(int num,int km)
{
  cout<<" SDevice setm50 "<<num<<" "<<km<<endl;
  if (openm50(num)==0) return km_m50(num,km);
  return 1;
}
int SDevice::getm50(int num)
{
 char  buf[10];
 int   dd;
 bool  fl;
  cout<<" SDevice getm50"<<endl;
  int t=read(hm50[num],buf,10);
  buf[ t ] = '\0';
  if (t<0) return -1;
  cout << " read answer 1 = "<<t<< "\tbuf:" << buf << endl;
  /*cout << "Answer: " << (int)buf[ 0 ] << '-' << (int)buf[ 1 ] << endl;
  if (t==4) {
   t=write(hm50[num],"m",1);
   sleep(1);
   //sleep(1);
   //sleep(1);
   t=read(hm50[num],buf,10);
   cout << " read answer 2 = "<<t<<endl;
  }*/
  //Alex 10.01.2012
  cout << "Answer: " << (unsigned int)buf[ 0 ] << '-' << (unsigned int)buf[ 1 ] << endl;
  //Обработка серийного номера на команду 'y'
  bool f_m50y = false;
  if( buf[ 0 ] < '0' )  //Приходит символ '*' в случае неверной команды
  {
      cout << "Read new serial number" << endl;
      t = write( hm50[ num ], "y", 1 );
      sleep( 1 );
      t = read( hm50[ num ], buf, 10 );
      cout << " read=" << t << "\tbuf:" << buf << endl;
      f_m50u = false;
      f_m50y = true;
  }
  //

  dd=0;
  fl=false;
  for(int i=0;i<t;i++) {
   int d=(int)buf[i];
   if (d>47){
    fl=true;    
    if (d>64) d=d-55;
    else d=d-48;    
    dd=dd*16+d;
    } 
  cout << "bf=" <<buf[i]<<endl;    
 }
 if(fl==false) return -1;
 
 if (f_m50u==true) dd+=256;
 if( f_m50y )   //Alex 10.01.2012
 {
     dd += 512;
     cout << "New serial number = " << dd << endl;
 }
 return dd;
 
}
int SDevice::openm50(int num)
{
 char* st;
 if (fm50[num]==true) close(hm50[num]);//return 0;
 switch (num) {
  case 0:{st="/dev/ttyS0";break;}
  case 1:{st="/dev/ttyS1";break;}
  case 2:{st="/dev/ttyS2";break;}
  case 3:{st="/dev/ttyS3";break;}
 } 
 hm50[num]=open(st,O_RDWR | O_NOCTTY | O_NDELAY);
 if (hm50[num]<0) return -1;
 fm50[num]=true;
 
 fcntl(hm50[num],F_SETFL,0);
 tcgetattr(hm50[num],&options);
 cfsetispeed(&options,B4800);
 cfsetospeed(&options,B4800);
 
 options.c_cflag&=~PARENB;
 options.c_cflag&=~CSTOPB;
 options.c_cflag&=~CSIZE;
 options.c_cflag|=CS8;
 options.c_cflag|=(CLOCAL | CREAD);
 options.c_lflag&=~(ICANON | ECHO | ECHOE | ISIG);
 options.c_oflag&=~OPOST;
 
 options.c_cc[VMIN]=0;
 options.c_cc[VTIME]=1;
 
 int t=tcsetattr(hm50[num],TCSANOW,&options);
 if (t<0) return -2;

 cout << st <<" "<< hm50[num] <<endl;
 return 0;
}
int SDevice::readadc(int w)
{
 w&=0xfe;
// outb(w,ba2+10);
 outb_("SDevice::readadc: ", w,ba2+10);
 w|=0x1;
// outb(w,ba2+10);
 outb_("SDevice::readadc: ", w,ba2+10);
 int t,t2;
// for(int i=0;i<10;i++) t=inb(ba2+10);
 for(int i=0;i<10;i++) t=inb_("SDevice::readadc: ", ba2+10);
 if ((t & 1)!=0) return 0xffff;
// t=inb(ba2+11);
// t2=inb(ba2+12);
 t=inb_("SDevice::readadc: ", ba2+11);
 t2=inb_("SDevice::readadc: ", ba2+12);
 //cout<<" t = "<<t<<" t2="<<t2<<endl;
 t2<<=8;
 t+=t2;
 //cout<<" t = "<<t<<" t2="<<t2<<endl;
 return t;
}
int SDevice::adc(int ch) 
{
 ch<<=5;
 ch|=16;
 int d=readadc(ch);
 if (d==0xffff) return 0;
 d&=0xfff;
 d*=2;
 //cout<<"adc="<<d<<endl;
 return d;
}
void SDevice::initdac()
{
 outdac(0x60);
}
void SDevice::initdacchan(int ch)
{
 if (ch>3|ch<0) return;
 outdac(0x4010 | ch << 10);
}
void SDevice::setdac(int ch,int code)
{
 if (ch>3|ch<0) return;
 outdac(0x2000 | ch << 10 | code & 0x3ff); 
 outdac(0xA200 | ch << 10); 
}
void SDevice::outdac(int d)
{
 sf9|=0xC8;
// outb(sf9,ba2+9);
 outb_("SDevice::outdac: ", sf9,ba2+9);
 sf9&=(~8);
// outb(sf9,ba2+9);
 outb_("SDevice::outdac: ", sf9,ba2+9);
 for (int i=0;i<16;i++){
  if((d & 0x8000)>0) sf9|=0x80;
  else sf9&=~0x80;
//  outb(sf9,ba2+9);
//  outb(sf9,ba2+9);
  outb_("SDevice::outdac: ", sf9,ba2+9);
  outb_("SDevice::outdac: ", sf9,ba2+9);
  if (i!=15) {
   sf9&=~0x40;
   d<<=1;
  }else sf9=sf9 & (~0x40) | 8;
// outb(sf9,ba2+9); 
// outb(sf9,ba2+9);
 outb_("SDevice::outdac: ", sf9,ba2+9); 
 outb_("SDevice::outdac: ", sf9,ba2+9);
 sf9|=0x40;
// outb(sf9,ba2+9);
 outb_("SDevice::outdac: ", sf9,ba2+9);
 }
}
void SDevice::on_offsep(bool f)
{
 //if (f==true) sf2&=~16;
//     else sf2|=16;
 if (f==true) { sf2|=16;sf1|=0x80;}
     else { sf2&=~16;sf1&=~0x80;}
// outb(sf2,ba2+2);    
// outb(sf1,ba2+1);
 outb_("SDevice::on_offsep: ", sf2,ba2+2);    
 outb_("SDevice::on_offsep: ", sf1,ba2+1);
 cout<<"on_ofsep"<<endl;  
} 
void SDevice::on_offpren(bool f)
{
 if (f==true) sf2|=8;
     else sf2&=~8;
// outb(sf2,ba2+2);      
 outb_("SDevice::on_offpren: ", sf2,ba2+2);      
 cout<<"on_ofpren"<<endl; 
}
void SDevice::on_offexp(bool f,int ch)
{
 cout<<"on_ofpren"<<endl;
 if (f==true) {
  if (ch==0) sf2&=~64;
      else  sf2&=~128;
  sf2|=1; //2
 }
 else{
 if (ch==0) sf2|=64;
      else  sf2|=128;
   //if (((sf2 & (~64))==~64)||
   int w=sf2;
   //cout<<" w="<<w<<endl;
   w=w & (64 + 128);
   if (w==(64+128)) sf2&=~1; //~2
   //cout<<" w="<<w<<endl;
   //sf2&=~1;         
 }      
// outb(sf2,ba2+2);
 outb_("SDevice::on_offexp: ", sf2,ba2+2);
 //cout<<"on_exp"<<endl;       
}
void SDevice::wait(int n) 
{
// for (int i=0;i<n;i++) int t=inb(ba2+4);
 for (int i=0;i<n;i++) int t=inb_("SDevice::wait: ", ba2+4);
}
void SDevice::write_pr(int d)
{
 sf3&=~2; // write
// outb(sf3,ba2+3);
 outb_("SDevice::write_pr: ", sf3,ba2+3);
 wait(10);
 sf3|=8;  // RTS
 sf3&=~4; // CLK
// outb(sf3,ba2+3);
 outb_("SDevice::write_pr: ", sf3,ba2+3);
 wait(10); 
 for(int i=0;i<8;i++){
   if ((d&1)==1) sf3|=1;
    else sf3&=~1;
   d>>=1;
//   outb(sf3,ba2+3);
   outb_("SDevice::write_pr: ", sf3,ba2+3);
   wait(10);
   sf3|=4;
//   outb(sf3,ba2+3);
   outb_("SDevice::write_pr: ", sf3,ba2+3);
   wait(10);
   sf3&=~4;
//   outb(sf3,ba2+3);
   outb_("SDevice::write_pr: ", sf3,ba2+3);
   wait(10);
   //cout<<"dd= "<<sf3<<endl;
 }
 /*
 if (f==true){
  sf3&=~8;  // RTS
  sf3|=4; // CLK
  outb(sf3,ba2+3);
 } */
 wait(10); 
}
void SDevice::write_d(int d,int n)
{
 /*
 sf3&=~2; // write
 outb(sf3,ba2+3);
 wait(10);
 sf3|=8;  // RTS
 sf3&=~4; // CLK
 outb(sf3,ba2+3);
 wait(10); */

 for(int i=0;i<n;i++){
   if ((d&1)==1) sf3|=1;
    else sf3&=~1;
   d>>=1;
//   outb(sf3,ba2+3);
   outb_("SDevice::write_d: ", sf3,ba2+3);
   wait(10);
   sf3|=4;
//   outb(sf3,ba2+3);
   outb_("SDevice::write_d: ", sf3,ba2+3);
   wait(10);
   sf3&=~4;
//   outb(sf3,ba2+3);
   outb_("SDevice::write_d: ", sf3,ba2+3);
   wait(10);
   //cout<<"dd= "<<sf3<<endl;
 }
  sf3&=~8;  // RTS
  sf3|=4; // CLK
//  outb(sf3,ba2+3);
  outb_("SDevice::write_d: ", sf3,ba2+3);
  wait(10); 
}

int SDevice::read_pr(int d)
{
 int t;
 int date=0;
 
 sf3|=2; // read
// outb(sf3,ba2+3);
 outb_("SDevice::read_pr: ", sf3,ba2+3);
 wait(10);
 sf3|=8;  // RTS
 sf3&=~4; // CLK
// outb(sf3,ba2+3);
 outb_("SDevice::read_pr: ", sf3,ba2+3);
 wait(10); 
 for(int i=0;i<d;i++){
   sf3|=4;
//   outb(sf3,ba2+3);
   outb_("SDevice::read_pr: ", sf3,ba2+3);
   wait(10);
//   t=(~inb(ba2+4))&1;
   t=(~inb_("SDevice::read_pr: ", ba2+4))&1;
   t<<=i;
   //date<<=1;
   date|=t;
   sf3&=~4;
//   outb(sf3,ba2+3);
   outb_("SDevice::read_pr: ", sf3,ba2+3);
   wait(10);
 }
 sf3&=~8;  // RTS
 sf3|=4; // CLK
// outb(sf3,ba2+3);
 outb_("SDevice::read_pr: ", sf3,ba2+3);
 wait(10); 
 return date; 
}
double SDevice::read_tm()
{
 sf3&=~8;
 sf3|=4;
// outb(sf3,ba2+3);
 outb_("SDevice::read_tm: ", sf3,ba2+3);
 wait(100);
 sf3&=~4;
// outb(sf3,ba2+3);
 outb_("SDevice::read_tm: ", sf3,ba2+3);
 wait(10000);
 
 write_pr(0xAA);
 int d=read_pr(9);
 if ((d & 0x100)>0) d|=0xff00;
 double dd=d;
 dd=dd/2;
 cout<<" d="<<d<<" dd="<<dd<<endl;           
 sf3&=~4;
// outb(sf3,ba2+3);
 outb_("SDevice::read_tm: ", sf3,ba2+3);
 return dd;
}
double SDevice::read_lt()
{
 write_pr(0xA2);
 int d=read_pr(9);
 if ((d & 0x100)>0) d|=0xff00;
 double dd=d/2;
 cout<<" d="<<d<<" dd="<<dd<<endl;           
 sf3&=~4;
// outb(sf3,ba2+3);
 outb_("SDevice::read_lt(): ", sf3,ba2+3);
 return dd;
}
double SDevice::read_ht()
{
 write_pr(0xA1);
 int d=read_pr(9);
 if ((d & 0x100)>0) d|=0xff00;
 double dd=d/2;
 cout<<" d="<<d<<" dd="<<dd<<endl;           
 sf3&=~4;
// outb(sf3,ba2+3);
 outb_("SDevice::read_ht(): ", sf3,ba2+3);
 return dd;
}
void SDevice::set_lht(int l,int h)
{
 l*=2;
 h*=2;
 write_pr(0x02);
 write_d(l,9);
 wait(10000); // ~20 mlc
 write_pr(0x01);
 write_d(h,9);
 wait(10000); // ~20 mlc
 sf3&=~4;
// outb(sf3,ba2+3);
 outb_("SDevice::set_lht: ", sf3,ba2+3);
}
void SDevice::setmode(int mode) 
{
 write_pr(0xC);
 write_d(mode,8);
 sf3&=~4;
// outb(sf3,ba2+3);
 outb_("SDevice::setmode: ", sf3,ba2+3);
}
void SDevice::start_puw(int ust) 
{
// outb(0x7A,ba2+7);
 outb_("SDevice::start_puw: ", 0x7A,ba2+7);
 int d=ust & 0x00FF;
// outb(d,ba2+5);
 outb_("SDevice::start_puw: ", d,ba2+5);
 d=(ust >> 8)&0x00FF;
// outb(d,ba2+5);
 outb_("SDevice::start_puw: ", d,ba2+5);
 sf3|=64;
// outb(sf3,ba2+3);
 outb_("SDevice::start_puw: ", sf3,ba2+3);
 sf1|=4;
// outb(sf1,ba2+1);
 outb_("SDevice::start_puw: ", sf1,ba2+1);
 wait(10000);
 sf2&=~32;
// outb(sf2,ba2+2);
 outb_("SDevice::start_puw: ", sf2,ba2+2);
}
void SDevice::set_puw(int ust) 
{
 int d=ust & 0x00FF;
 //cout<<"PUW  d="<<d<<endl;
// outb(d,ba2+5);
 outb_("SDevice::set_puw: ", d,ba2+5);
 d=(ust >> 8)&0x00FF;
 //cout<<"PUW d="<<d<<endl;
// outb(d,ba2+5);
 outb_("SDevice::set_puw: ", d,ba2+5);
}
void SDevice::stop_puw() 
{
 sf3&=~64;
// outb(sf3,ba2+3);
 outb_("SDevice::stop_puw(): ", sf3,ba2+3);
 sf1&=~4;
// outb(sf1,ba2+1);
 outb_("SDevice::stop_puw(): ", sf1,ba2+1);
 sf2|=32;
// outb(sf2,ba2+2);
 outb_("SDevice::stop_puw(): ", sf2,ba2+2);
}
void SDevice::set_rgu(int rgu) 
{
 int w;
 switch (rgu){
  case 0:{w=sf1;break;}
  case 1:{w=sf1  & (~2);break;}           // hi
  case 2:{w=(sf1 | 1) & (~2);break;}     // lo  
 } 
 //cout<<" PGU= "<<w<<endl;
// outb(w,ba2+1);
 outb_("SDevice::set_rgu: ", w,ba2+1);
}
void SDevice::set_im(int n,bool f)
{
 int w=1;
 if (n<4) w<<=4+n;
 else w<<=n-4;
 //cout<<"n= "<< n<< " w="<<w<<endl;
 //w=0xff;
 if (f==true) sf0|=w;
      else    sf0&=~w;
// outb(sf0,ba2);      
 outb_("SDevice::set_im: ", sf0,ba2);      
}
void SDevice::on_offiw(bool f)
{
 if (f==true) sf1|=16;
        else  sf1&=~16;
// outb(sf1,ba2+1);      
 outb_("SDevice::on_offiw: ", sf1,ba2+1);      
}
void SDevice::on_offosw(bool f)
{
 if (f==true) sf2|=4;
        else  sf2&=~4;
// outb(sf2,ba2+2);      
 outb_("SDevice::on_offosw: ", sf2,ba2+2);      
}
int SDevice::get_rgu2()
{
 int rgu[3] = {0,0,0};
 for (int i=0;i<100;i++){ 
// int t=inb(ba2+4);
 int t=inb_("SDevice::get_rgu2: ",ba2+4);
 //cout<<"RGU t="<<t<<endl;
  if ((t & 0x80) ==0)  rgu[2]++;
   else if ((t & 0x40) ==0)  rgu[1]++;
           else rgu[0]++;    
 } 
 int m=rgu[0];
 int r=0;
 for (int i=0;i<3;i++){
  if (rgu[i]>m) { m=rgu[i];r=i;} 
 }
 cout << "RGU = " <<r<<endl;
 return r;
}
double SDevice::get_im()
{
// int i=inb(ba2);
 int i=inb_("SDevice::get_im(): ", ba2);
 //i=100;
 return (double)i;
}
void SDevice::initada()
{
// afifo=inw(ba) & 0x03ff;
 afifo=inw_("SDevice::initada(): ", ba) & 0x03ff;
 fifo=afifo;
 sfw0=0;
 sfw6=0;
// outw(sfw0 | 0x20,ba);
 outw_("SDevice::initada(): ", sfw0 | 0x20,ba);
 sfw0=sfw0 | 0x10;
// outw(sfw0,ba);
// outw(sfw0 | 0x40,ba);
 outw_("SDevice::initada(): ", sfw0,ba);
 outw_("SDevice::initada(): ", sfw0 | 0x40,ba);
 sfw0=sfw0 | 0x0f;
// outw(sfw0,ba);
// outw(1,ba+8);
 outw_("SDevice::initada(): ", sfw0,ba);
 outw_("SDevice::initada(): ", 1,ba+8);
 cout << "INIT ADA " <<endl;
 initdac();
 for(int i=0;i<4;i++) initdacchan(i);
 for(int i=0;i<4;i++) setdac(i,0);
}
int SDevice::getblk()
{
//int b=inb(ba2+4);
int b=inb_("SDevice::getblk(): ", ba2+4);
return b;
}
