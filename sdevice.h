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
#ifndef SDEVICE_H
#define SDEVICE_H
#include <iostream>
#include <cstdlib>
#include <sys/io.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>
/**
@author and
*/
const int BADDR  = 0x300;
const int BADDR2 = 0x360;
const int SizeFifo  = 1024;
class SDevice{
public:
    int sizebuff;
    unsigned short buff[SizeFifo];
    unsigned short kod[4];
    SDevice();
    ~SDevice();
    void getbuff();
    void setudeu();
    void setptdeu();
    int  setm50(int num,int km);
    int  getm50(int num);
    int  adc(int ch);
    void setdac(int ch,int code);
    void on_offsep(bool f);
    void on_offpren(bool f);
    void on_offexp(bool f,int ch);
    void on_offiw(bool f);
    void on_offosw(bool f);
    double  read_tm();
    double  read_ht();
    double  read_lt();
    void set_lht(int l,int h);
    void start_puw(int ust);
    void set_puw(int ust); 
    void stop_puw();
    void set_rgu(int rgu);
    void set_im(int n,bool f);
    int  get_rgu2();
    double get_im();
    void initada();
    int getblk();
private:
   int ba; 
   int ba2;
   int fifo,afifo;
   struct termios options;
   unsigned short sfw0,sfw6;     
   unsigned short sf0i,sf0r,sf0;
   unsigned short sf1,sf2,sf3;
   unsigned short sf4,sf5,sf6;
   unsigned short sf7,sf8,sf9;
   unsigned short sf10,sf11,sf12;
   
   bool fm50[4];
   int  hm50[4];  
   int  openm50(int num);
   int  km_m50(int num,int km);
   int  readadc(int w);
   
   void initdac();
   void initdacchan(int ch);
   void outdac(int d);
   void write_pr(int d);
   void write_d(int d,int n);
   int  read_pr(int d);
   void setmode(int mode);
   void wait(int n);
   void outb_(const char *_metod, unsigned char _value, unsigned short int _port){
       std::cout << _metod << "outb(" << _value <<"," << _port <<")" << std::endl;
   }
   void outw_(const char *_metod, unsigned short int _value, unsigned short int _port){
       std::cout << _metod << "outb(" << _value <<"," << _port <<")" << std::endl;
   }
   unsigned short int inw_(const char *_metod, unsigned short int _port){
       std::cout << _metod << "inw(" << _port <<")" << std::endl;
       return 0;
   }
   unsigned char inb_(const char *_metod, unsigned short int _port){
       std::cout << _metod << "inw(" << _port <<") "<< std::endl;
       return 0;
   }

};

#endif
