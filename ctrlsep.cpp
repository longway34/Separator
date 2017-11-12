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
#include "ctrlsep.h"

#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
FILE *out;
//ofstream ofile;
using namespace std;
 
CtrlSep::CtrlSep()
{
 cout << "Hello, ctrlsep!" << endl;
 error=OK;
 state=WAIT_C;
 f_sepon=false;
 f_sepwon=false;
 f_log=false;
 
 //out=fopen("Separlog","a");
 //fprintf(out,"  Open Log  \n");
 //fclose(out);
 //if (out!=NULL){
 // printf(out," Begin Log\n");
 //}
}


CtrlSep::~CtrlSep()
{
 //fclose(out);
}




/*!
    \fn CtrlSep::RunSep()
 */
void CtrlSep::RunSep()
{
    /// @todo implement me
char* wr;
//pthread_t thread_id;
//pthread_create(&thread_id,NULL,&mySepar.execute,NULL); 
mySepar.sock=myServer.listener;
 
//sleep(15);
cout << "Socket open "<< endl;   
if (myServer.SetSocket()) cout << "Socket on" << endl;
 else {cout << "Socket error "<< endl;return;}
 while ( true){
 m1:
   cout << "Wait accept" << endl;
   mySepar.set_stopall();
   /* 
   myServer.getbuff(bf_in);
    int tm;
    char c=bf_in[0]; 
    memcpy(&tm,&bf_in[1],4);
    cout << (int)c<<" tm= " << tm << endl;*/
   if (myServer.my_accept()) cout << "accept" << endl;
     else {cout << "accept error" << endl;goto m1;break;}
   while ( true){
     if (myServer.getbuff()){
        
         //cout << "recive " << myServer.get_recive() << "  ==="<< (int) myServer.in_buff[0] << endl;
	 sep_kom();
      }
     else break;
   }
 }
}

void CtrlSep::sep_kom()
{
    /// @todo implement me
    //cout<<" IN  STATE = "<< state << " ERROR = " <<error<<endl;
struct timeval tv;    
struct tm* ptm;
char*  st;
char time_string[40];
   if (f_log){
     out=fopen("Separlog","a");
     int size=ftell(out);
     //cout<<"size = "<<size<<endl;
     if (size>100000) {
      fclose(out);
      out=fopen("Separlog","w");
     } 
     gettimeofday(&tv,NULL);
     ptm=localtime(&tv.tv_sec);
     strftime(time_string,sizeof(time_string),"%Y-%m-%d  %H:%M:%S",ptm);
     //printf("%s\n",time_string);
     fprintf(out,"%s\n",time_string);
     fprintf(out,"Command\n");
    }
    //cout<<"Command"<<endl;      
    bool f=true;
    if (mySepar.f_Reset==true){
       state=WAIT_C;
     }
    int k=(int)myServer.in_buff[0];
    //cout<<" km 0= "<<k<<" 1="<<(int)myServer.in_buff[1]<<" 2="<<(int)myServer.in_buff[2]<< endl;
    
    //cout<<"     Byte received =" << myServer.get_recive()<<endl; 
    cout<<"     Code = "<<k<<"  ";    
    /*
    if (f_log){
     fprintf(out,"     Byte received =%d\n",myServer.get_recive());
     fprintf(out,"     Code = %d\n",k);
    }*/
    switch (k) {
      case 0xA  : { getstate(); break;}
      case 0xB  : { testim();   break;}
      case 0xC  : { initada();  break;}
      case 0xD  : { getver();   break;}
      case 0xF  : { stoptest(); break;}
      case 0x10 : { setspk();   break;}
      case 0x11 : { getspk();   break;}
      case 0x20 : { setsepar(); break;}
      case 0x21 : { getsepar(); break;}
      case 0x22 : { startsep(); break;}
      case 0x23 : { stopsep();  break;}
      case 0x24 : { getkspk();  break;}
      case 0x25 : { getgist();  break;}
      case 0x27 : { startpuw(); break;}
      case 0x28 : { setpuw();   break;}
      case 0x29 : { stoppuw();  break;}
      case 0x30 : { setudeu();  break;}
      case 0x31 : { setren();   break;}
      case 0x32 : { getren();   break;}
      case 0x33 : { onren();    break;}
      case 0x34 : { offren();   break;}
      case 0x35 : { setrgu();   break;}
      case 0x37 : { settemp();  break;}
      case 0x38 : { gettemp();  break;}
      case 0x40 : { expon();    break;}
      case 0x41 : { expoff();   break;}
      case 0x50 : { onsep();    break;}
      case 0x51 : { offsep();   break;}
      case 0x52 : { oniw();     break;}
      case 0x53 : { offiw();    break;}
      case 0x54 : { onosw();    break;}
      case 0x55 : { offosw();   break;}
      case 0x56 : { setrgu2();  break;}
      case 0x57 : { getrgu2();  break;}
      case 0x58 : { setim();    break;}
      case 0x63 : { setptdeu(); break;}
      case 0x65 : { setm50();   break;}
      case 0x66 : { getm50();   break;}
      case 0x67 : { getblk();   break;}
      case 0x68 : { setlog();   break;} 
      default   : {  no_km();   f=false; break;}
    }
 
    if(f==true) {
     mySepar.reset(); 
    }
 cout<<" OUT  STATE = "<< state << " ERROR = " <<error<<endl;     	
 st="  ERROR";    
 switch (error) {
   case 0 : st=" Ok"; break;
   case 1 : st=" ERROR[code =1] Missing command"; break;
   case 2 : st=" ERROR[code =2] Incorrect data"; break;
   case 3 : st=" ERROR[code =3] Rang check of data"; break;
   case 4 : st=" ERROR[code =4] Forbidden command"; break;
   case 5 : st=" ERROR[code =5] Impossible command"; break;  
 }  
 //cout<<info<<endl;
 /*
 cout<<"Answer: "<<st<<endl; 
 cout<<"     ByteSend ="<< myServer.byte_send<<endl;   
 if (f_log && (out!=NULL)){
  fprintf(out,"%s\n",info);
  fprintf(out,"Answer: %s\n",st);
  fprintf(out,"     ByteSend =%d\n",myServer.byte_send);
  fclose(out);
 } 
 cout<<" OUT  STATE = "<< state << " ERROR = " <<error<<endl;*/
 
}
void CtrlSep::getstate()
{
 //cout <<"     Getstate" << endl;
 //fprintf(out,"     Getstate\n");
 info="     Getstate";
 int sz=2;
 myServer.out_buff[0]=0;//error
 myServer.out_buff[1]=state;
 myServer.set_send(sz);
 myServer.sendbuff();
}
void CtrlSep::no_km()
{
 
 //cout <<"     No km" << endl;
 //fprintf(out,"     No km\n");
 info="     No km";
 int sz=1;
 myServer.out_buff[0]=N_KM;//error
 myServer.set_send(sz);
 myServer.sendbuff();
}
 
void CtrlSep::startsep()
{
 error=0;
 //cout << "     Startsep" << endl; 
 //fprintf(out,"     Startsep\n");
 info="     Startsep";
 if (state==WAIT_C && f_sepon==true){
  //mySepar.initada();
  mySepar.startsep();
  state=SEP;
  f_sepwon=true;
 }
 else error=NN_EX; 
 myServer.out_buff[0]=error;
 myServer.set_send(1);
 myServer.sendbuff();
 
}
void CtrlSep::stopsep()
{
 error=0;
 //cout << "     Stopsep" << endl; 
 //fprintf(out,"     Stopsep\n");
 info="     Stopsep";
 if (state==SEP || state==WAIT_C){
  mySepar.stopsep();
  state=WAIT_C;
 }
 else error=NN_EX;
 myServer.out_buff[0]=error;
 myServer.set_send(1);
 myServer.sendbuff(); 
}
void CtrlSep::getgist()
{
 //cout << "     Getgist" << endl;
 //fprintf(out,"     Getgist\n");
 info="     Getgist";
 error=0;
  unsigned short channel=0;
  if (myServer.get_recive()==2){
   channel=myServer.in_buff[1];
   if (channel<mySepar.getchannel()) {
     if (state==SEP || f_sepwon==true);
     else error=NN_EX;
    }
    else error=N_DATE;
   }
   else error=N_D_KM;
   int sz=1;
   myServer.out_buff[0]=error;
   
   if (error==0) {
    //mySepar.timekspk[channel]=100;
    //for(int i=0;i<256;i++) mySepar.kch[channel]->spk[i]=i;
    //int tm=(int)mySepar.timekspk[channel]*4;
    int szg=mySepar.getszgist();
     cout << " szgist="<<szg<<endl;
    //mySepar.gch[channel]->gist[10]=100;
    //mySepar.gch[channel]->gist[20]=200;

    memcpy(&myServer.out_buff[1],&mySepar.gch[channel]->gist[0],szg);
    sz+=szg;
    //mySepar.clrkspk(channel);
   }    
   myServer.set_send(sz);
   myServer.sendbuff();
 
}
void CtrlSep::getsepar()
{ 
 error=0;
 //cout << "     Getsepar" << endl; 
 //fprintf(out,"     Getsepar\n");
 info="     Getsepar";
 int sz=sizeof(mySepar.sep_work);
 //cout << " size sz ??? " << sz << endl;
 //if (f_sepwon==true){
  memcpy(&myServer.out_buff[1],&mySepar.sep_work,sz);
  //}
  //else error=NN_EX;
  myServer.out_buff[0]=error;
  myServer.set_send(sz+1);
  myServer.sendbuff();
}
void CtrlSep::setsepar()
{
 cout << "     Setsepar " << endl; 
 //fprintf(out,"     Setsepar\n");
 info="     Setsepar";
 error=0;
 int sz=mySepar.sizeUst();//sizeof(mySepar.wust);
 int in_sz=myServer.get_recive();
 cout<<" sz="<<sz<<" in_sz="<<in_sz<<endl;
 if (myServer.get_recive()==(sz+1)){
    if (state==WAIT_C || state==SEP){
     //memcpy(&mySepar.ust,&myServer.in_buff[1],sz);  
     memcpy(&*mySepar.wust,&myServer.in_buff[1],sz);
     if (mySepar.test_ust()==true) f_sepon=true;
     else error=N_DATE;
    } 
    else error=N_EX;
   }
 else error=N_D_KM;
 
 myServer.out_buff[0]=error;
 myServer.set_send(1);
 myServer.sendbuff();
 
 //memcpy(&mySepar.ust,&myServer.in_buff[1],sz);
 
 //cout << "obl= " << mySepar.wust->obl[0][0].ls << endl;
 //cout << "ust= " << mySepar.wust->usl[0] << endl;
 
 //cout << "obl[0][0] rs " << mySepar.wust->obl[0][0].rs << endl;
 //cout << "obl[0][0] ls " << mySepar.wust->obl[0][0].ls << endl;
 //cout << "obl " << mySepar.ust.obl[3][3].rs << endl;
 //cout << "gmz[0] " << mySepar.wust->gmz[0] << endl;
 //cout << "gmz[1] " << mySepar.wust->gmz[1] << endl;
 for (int i=0;i<6;i++)
  cout << "sh3 " << mySepar.wust->sh3[i]<< " ";
 cout<<endl;
 cout << "kh3[0] " << mySepar.wust->kh3[0] << " kh3[1] "<<mySepar.wust->kh3[1]<<endl;
 
 for (int i=0;i<4;i++) {
  for (int j=0;j<6;j++)
  cout << "prg["<<i<<"]["<<j<<"]" << mySepar.wust->prg[i][j] <<" ";
  cout<<endl;
 }
 
 for (int i=0;i<4;i++) {
  for (int j=0;j<6;j++)
  cout << "prg2["<<i<<"]["<<j<<"]" << mySepar.wust->prg2[i][j] <<" ";
  cout<<endl;
 }
 
 cout << "b_zd 0 0" << mySepar.wust->b_zd[0][0] << endl;
 cout << "b_zd 1 0" << mySepar.wust->b_zd[1][0] << endl;
 cout << "kPrDMin " << mySepar.wust->kprMin << " kPrDMax " << mySepar.wust->kprMax<< endl;
 cout << "Alg " << mySepar.wust->alg << endl;
 cout << "sep_row " << mySepar.wust->sep_row << endl;

 for (int i=0;i<4;i++) {
  cout << "gmz[0] " << mySepar.wust->gmz[i] << endl;
 } 
 //cout << "prg " << mySepar.ust.prg[3][3] << endl;
 //cout << "tz 0 " << mySepar.wust->tz[0] << endl;
 //cout << "fh12 " << mySepar.wust->fh12 << endl;
 //cout << "gcol " << mySepar.wust->gcol << endl;
 //cout << "usl[0] " << mySepar.wust->usl[0] << endl;
 //cout << "totb " << mySepar.wust->totb << endl;
 //cout << "b_zd " << mySepar.ust.b_zd[3] << endl;
 
 

}
void CtrlSep::setudeu()
{
 //cout << "     Setudeu" << endl;
 //fprintf(out,"     Setudeu\n"); 
 info="     Setudeu";
 error=0;
 int in_sz=sizeof(mySepar.kod[0])*mySepar.getchannel();
 if (myServer.get_recive()==(in_sz+1)){
     if (state==WAIT_C){
      memcpy(&mySepar.kod[0],&myServer.in_buff[1],in_sz);
      for (int i=0;i<mySepar.getchannel();i++){
      if (mySepar.kod[i]>255) error=N_DATE;
       cout<< mySepar.kod[i] << endl;
      } 
     }
     else error=N_EX;
  }
 else error=N_D_KM;
 //cout << " ER=" << error << endl;
 myServer.out_buff[0]=error;
 myServer.set_send(1);
 myServer.sendbuff();
 if (error==0) mySepar.setudeu();
}
void CtrlSep::setptdeu()
{
 //cout << "     Setptdeu" << endl; 
 //fprintf(out,"     Setptdeu\n");
 info="     Setptdeu";
 error=0;
 int in_sz=sizeof(mySepar.kod[0])*mySepar.getchannel();
 if (myServer.get_recive()==(in_sz+1)){
     if (state==WAIT_C){
      memcpy(&mySepar.kod[0],&myServer.in_buff[1],in_sz);
      for (int i=0;i<mySepar.getchannel();i++){
      if (mySepar.kod[i]>255) error=N_DATE;//mySepar.kod[i]=128;
      //cout<< mySepar.kod[i] << endl;
      } 
     }
     else error=N_EX;
  }
 else error=N_D_KM;
 myServer.out_buff[0]=error;
 myServer.set_send(1);
 myServer.sendbuff();
 if (error==0) mySepar.setptdeu();
}
void CtrlSep::setspk()
{
    int tm;
    error=0;
 //cout << "     Setspk" << tm << endl;
 //fprintf(out,"     Setspk\n");    
 info="     Setspk";
    if (myServer.get_recive()==sizeof(tm)+1){
      memcpy(&tm,&myServer.in_buff[1],4);
     if (tm>mySepar.getmintime() && tm<= mySepar.getmaxtime()){
       if (state==WAIT_C);
       else error=N_EX;     
      }
      else error=N_DATE; 
    } 
    else error=N_D_KM; 
         
    myServer.out_buff[0]=error;
    myServer.set_send(1);
    myServer.sendbuff();
    if (error!=0) return;
    //cout << "setspk  time=" << tm << endl; 
    //mySepar.initada();
    mySepar.setspk(tm);
    state=SPK;
}

void CtrlSep::getspk()
{
 //ut << "     Getspk"  <<  endl; 
 //rintf(out,"     Getspk\n");
  info="     Getspk"; 
  error=0;
  unsigned short channel=0;
  if (myServer.get_recive()==2){
   channel=myServer.in_buff[1];
   if (channel<mySepar.getchannel()) {
     if (state==SPK || state==WAIT_C){
       if (mySepar.flag_spk()==true);
        else error=N_EX;
     }
     else error=NN_EX;
    }
    else error=N_DATE;
   }
   else error=N_D_KM;
   int sz=1;
   myServer.out_buff[0]=error;
   
   if (error==0) {
    int szspk=mySepar.getszspk();
    memcpy(&myServer.out_buff[1],&mySepar.ch[channel]->spk[0],szspk);
    sz+=szspk;
    state=WAIT_C;
   }    
   myServer.set_send(sz);
   myServer.sendbuff();
  //cout << " channel " << channel << endl;
}
void CtrlSep::setm50()
{
 error=0;
 //cout << "     Setm50  "<<endl;
 //fprintf(out,"     Setm50\n");
 info="     Setm50";
 int num;
 int km;
 if (myServer.get_recive()==3){
    num=myServer.in_buff[1];
    km=myServer.in_buff[2];
  
  if(((num==0) || (num==1))&&((km==0)  || (km==1) || ((km>=10)&&(km<=19)))){ 
      if (state==WAIT_C) ;
      else error=NN_EX;
   }  else error=N_DATE;
  }   else error=N_D_KM;
 
 int sz=1;
 myServer.out_buff[0]=error;
 if (error==0){           
  
  mySepar.setm50(num,km);
  //cout << "setm50  " <<num<<" "<<km<< endl;
 } 
 myServer.set_send(sz);
 myServer.sendbuff();
}
void CtrlSep::getm50()
{
 error=0;
 //cout << "     Getm50  " << endl;
 //fprintf(out,"     Getm50\n");
 info="     Getm50";
 //  " size="<<myServer.get_recive()<<endl;
 int num;
  if (myServer.get_recive()==2){
    num=myServer.in_buff[1];
    if((num==0) || (num==1)){ 
      if (state==WAIT_C) ;
      else error=NN_EX;
   }  else error=N_DATE;
  }   else error=N_D_KM;
 
 int sz=1;
 myServer.out_buff[0]=error;
 if (error==0){  
  int w=mySepar.getm50(num);
  memcpy(&myServer.out_buff[1],&w,2);
  sz=3;
 } 
 //cout<<" SEND M50  "<<(int)myServer.out_buff[1]<<"   "<<(int)myServer.out_buff[2] <<endl;
 myServer.set_send(sz);
 myServer.sendbuff(); 
 
}
void CtrlSep::getkspk()
{ 
 //cout << "     Getkspk  " << endl;
 //fprintf(out,"     Getkspk\n");
 info="     Getkspk";
  error=0;
  unsigned short channel=0;
  if (myServer.get_recive()==2){
   channel=myServer.in_buff[1];
   if (channel<mySepar.getchannel()) {
     if (state==SEP);
     else error=NN_EX;
    }
    else error=N_DATE;
   }
   else error=N_D_KM;
   int sz=1;
   myServer.out_buff[0]=error;
   
   if (error==0) {
    //mySepar.timekspk[channel]=100;
    //for(int i=0;i<256;i++) mySepar.kch[channel]->spk[i]=i;
    int tm=(int)mySepar.timekspk[channel]*4;
    int szspk=mySepar.getszspk();
    memcpy(&myServer.out_buff[1],&tm,sizeof(tm));
    memcpy(&myServer.out_buff[5],&mySepar.kch[channel]->spk[0],szspk);
    sz+=szspk;
    sz+=sizeof(tm);
    mySepar.clrkspk(channel);
   }    
   myServer.set_send(sz);
   myServer.sendbuff();
  //cout << " channel " << channel << endl;
}
void CtrlSep::startpuw()
{
  error=0;
 //cout << "     Startpuw" << endl; 
 //fprintf(out,"     Startpuw\n");
 info="     Startpuw";
 if (state==WAIT_C || state==SEP){
  //start
 }
 else error=N_EX; 
 myServer.out_buff[0]=error;
 myServer.set_send(1);
 myServer.sendbuff();
 if (error==0) mySepar.start_puw();
}
void CtrlSep::setpuw()
{
  error=0;
  //cout << "    Stetpuw" << endl;
  //fprintf(out,"     Stetpuw\n");
  info="     Setpuw";
  unsigned short ust=0; 
  if (myServer.get_recive()==sizeof(ust)+1){
      memcpy(&ust,&myServer.in_buff[1],2); 
  if (state==WAIT_C || state==SEP){
  //setpuw
   //cout<<"UST="<<ust<<endl;
  }
  else error=N_EX; 
 }
 else error=N_D_KM; 
 myServer.out_buff[0]=error;
 myServer.set_send(1);
 myServer.sendbuff();
 if (error==0) mySepar.set_puw(ust);
}
void CtrlSep::stoppuw()
{
 error=0;
 //cout << "     Stoppuw" << endl; 
 //fprintf(out,"     Stoppuw\n");
 info="     Stoppuw";
 if (state==WAIT_C || state==SEP){
  //start
 }
 else error=N_EX; 
 myServer.out_buff[0]=error;
 myServer.set_send(1);
 myServer.sendbuff();
 if (error==0) mySepar.stop_puw();
  
}
void CtrlSep::onren()
{
  error=0;
 //cout << "     ONren" << endl; 
 //fprintf(out,"     ONren\n");
 info="     ONren";
 if (state==WAIT_C){
 }
 else error=N_EX; 
 myServer.out_buff[0]=error;
 myServer.set_send(1);
 myServer.sendbuff();
 if (error==0) mySepar.on_offpren(true);
}
void CtrlSep::offren()
{
  error=0;
 //cout << "     OFFren" << endl; 
 //fprintf(out,"     OFFren\n");
 info="     OFFren";
 if (state==WAIT_C){
  //onren
 }
 else error=N_EX; 
 myServer.out_buff[0]=error;
 myServer.set_send(1);
 myServer.sendbuff();
 if (error==0) mySepar.on_offpren(false);
}
void CtrlSep::onsep()
{
  error=0;
 //cout << "     ONsep" << endl; 
 //fprintf(out,"     ONsep\n");
 info="     ONsep";
 if (state==WAIT_C){
  
 }
 else error=N_EX; 
 myServer.out_buff[0]=error;
 myServer.set_send(1);
 myServer.sendbuff();
 if (error==0) mySepar.on_offsep(true);
}
void CtrlSep::offsep()
{
  error=0;
 //cout << "     OFFsep" << endl; 
 //fprintf(out,"     OFFsep\n");
 info="     OFFsep";
 if (state==WAIT_C){
  //offsep
 }
 else error=N_EX; 
 myServer.out_buff[0]=error;
 myServer.set_send(1);
 myServer.sendbuff();
 if (error==0) mySepar.on_offsep(false);
}
void CtrlSep::oniw()
{
 error=0;
 //cout << "     ONiw" << endl; 
 //fprintf(out,"     ONiw\n");
 info="     ONiw";
 if (state==WAIT_C || state==SEP){
  //offsep
 }
 else error=N_EX; 
 myServer.out_buff[0]=error;
 myServer.set_send(1);
 myServer.sendbuff();
 if (error==0) mySepar.on_offiw(true);
}
void CtrlSep::offiw()
{
 error=0;
 //cout << "     OFFiw" << endl; 
 //fprintf(out,"     OFFiw\n");
 info="     OFFiw";
 if (state==WAIT_C || state==SEP){
  //offsep
 }
 else error=N_EX; 
 myServer.out_buff[0]=error;
 myServer.set_send(1);
 myServer.sendbuff();
 if (error==0) mySepar.on_offiw(false);
}
void CtrlSep::onosw()
{
 error=0;
 //cout << "     ONosw" << endl; 
 //fprintf(out,"     ONosw\n");
 info="     ONosw";
 if (state==WAIT_C || state==SEP){
 }
 else error=N_EX; 
 myServer.out_buff[0]=error;
 myServer.set_send(1);
 myServer.sendbuff();
 if (error==0) mySepar.on_offosw(true);
}
void CtrlSep::offosw()
{
 error=0;
 //cout << "     OFFosw" << endl; 
 //fprintf(out,"     OFFosw\n");
 info="     OFFosw";
 if (state==WAIT_C || state==SEP){
 }
 else error=N_EX; 
 myServer.out_buff[0]=error;
 myServer.set_send(1);
 myServer.sendbuff();
 if (error==0) mySepar.on_offosw(false);
}
void CtrlSep::setren()
{
  unsigned short kw,mka,ch;
  error=0;
  //cout << "    Setren" << endl;
  //fprintf(out,"     Setren\n");    
  info="     Setren";
    if (myServer.get_recive()==sizeof(kw)+sizeof(mka)+2){
      memcpy(&kw,&myServer.in_buff[2],2);
      memcpy(&mka,&myServer.in_buff[4],2);
      ch=myServer.in_buff[1];
     if (ch==0 || ch== 1){
       if (state==WAIT_C);
       else error=N_EX;     
      }
      else error=N_DATE; 
    } 
    else error=N_D_KM; 
         
    myServer.out_buff[0]=error;
    myServer.set_send(1);
    myServer.sendbuff();
    if (error!=0) return;
     mySepar.setren(ch,kw,mka);
    cout << "setren  ch=" << ch <<" kw="<<kw<<" mka="<<mka<< endl; 
}
void CtrlSep::getren()
{
 error=0;
 //cout << "     Getren"  << endl;
 //fprintf(out,"     Getren\n");
 info="     Getren";
 unsigned short kw,mka,ch;
 if (myServer.get_recive()==2){
      ch=myServer.in_buff[1];
     if (ch==0 || ch== 1){
       if (state==WAIT_C);
       else error=N_EX;     
      }
      else error=N_DATE; 
    } 
    else error=N_D_KM; 
    int sz=1;
    if (error==0) {
     kw=200;mka=300;
     mySepar.getren();
     if (ch==0) {
      kw=mySepar.kw1;
      mka=mySepar.mka1;
     }
     else {
      kw=mySepar.kw2;
      mka=mySepar.mka2;
     }
     memcpy(&myServer.out_buff[1],&kw,2);
     memcpy(&myServer.out_buff[3],&mka,2);
     sz+=4;
    }     
    myServer.out_buff[0]=error;
    myServer.set_send(sz);
    myServer.sendbuff();
    cout<<"Getren ch="<<ch<<" kw="<<kw<<" mka="<<mka<<endl; 
}
void CtrlSep::settemp()
{
 unsigned short l,h;
 error=0;
 //cout << "     Settemp" << endl;
 //fprintf(out,"     Settemp\n");    
 info="     Settemp";
    if (myServer.get_recive()==sizeof(l)+sizeof(h)+1){
      memcpy(&l,&myServer.in_buff[1],2);
      memcpy(&h,&myServer.in_buff[3],2);
     if (l<=h){
       if (state==WAIT_C);
       else error=N_EX;     
      }
      else error=N_DATE; 
    } 
    else error=N_D_KM; 
         
    myServer.out_buff[0]=error;
    myServer.set_send(1);
    myServer.sendbuff();
    if (error!=0) return;
    mySepar.setterm(l,h);
    state=TERM; 
    //cout << "settemp l =" << l <<" h="<<h<< endl; 
}
void CtrlSep::gettemp()
{
 //cout << "     Gettemp"  << endl;
 //fprintf(out,"     Gettemp\n");
 info="     Gettemp";
 double l,h,t;
 error=0;
 if (myServer.get_recive()==1){
       if (state==TERM);
       else error=N_EX;     
    } 
    else error=N_D_KM; 
    int sz=1;
    if (error==0) {
     //l=20;h=30;t=24; 
     // getterm
     l=mySepar.loterm;
     h=mySepar.hiterm;
     t=mySepar.tterm;
     
     memcpy(&myServer.out_buff[1],&l,8);
     memcpy(&myServer.out_buff[9],&h,8);
     memcpy(&myServer.out_buff[17],&t,8);
     sz+=8*3;
     state=WAIT_C;
    }     
    myServer.out_buff[0]=error;
    myServer.set_send(sz);
    myServer.sendbuff();
}
void CtrlSep::setrgu()
{
 unsigned short rgu;
 error=0;
 //cout << "     Setrgu" << endl;
 //fprintf(out,"     Setrgu\n");    
 info="     Setrgu";
    if (myServer.get_recive()==2){
     rgu=myServer.in_buff[1];
     if (rgu==0 || rgu==1 || rgu==2){
       if (state==WAIT_C);
       else error=N_EX;     
      }
      else error=N_DATE; 
    } 
    else error=N_D_KM; 
         
    myServer.out_buff[0]=error;
    myServer.set_send(1);
    myServer.sendbuff();
    if (error!=0) return;
    mySepar.set_rgu(rgu);
    //cout << "setrgu  =" << rgu << endl; 
}
void CtrlSep::expon()
{
 unsigned short ch; 
 error=0;
 ch=myServer.in_buff[1];
 //cout << "     ONexp " <<ch<< endl; 
 //fprintf(out,"     ONexp\n");
 info="     ONexp";
     if (ch==0 || ch== 1){
       if (state==WAIT_C);
       else error=N_EX;     
      }
     else error=N_DATE; 
 myServer.out_buff[0]=error;
 myServer.set_send(1);
 myServer.sendbuff();
 if (error!=0) return;
 mySepar.on_offexp(true,ch);
}
void CtrlSep::expoff()
{
 unsigned short ch; 
 error=0;
  
 ch=myServer.in_buff[1];
 //cout << "OFFexp " <<ch<< endl; 
 //fprintf(out,"     OFFexp\n");
 info="     OFFexp";
     if (ch==0 || ch== 1){
       if (state==WAIT_C);
       else error=N_EX;     
      }
     else error=N_DATE; 
 myServer.out_buff[0]=error;
 myServer.set_send(1);
 myServer.sendbuff();
 if (error!=0) return;
 //offexp
 mySepar.on_offexp(false,ch);
}
void CtrlSep::testim()
{
 unsigned short gr,dl,ch;
 error=0;
 //cout << "     Testim" << endl;
 //fprintf(out,"     Testtim\n");
 info="     Testim";
    
    if (myServer.get_recive()==sizeof(gr)+sizeof(dl)+2){
      memcpy(&gr,&myServer.in_buff[2],2);
      memcpy(&dl,&myServer.in_buff[4],2);
      ch=myServer.in_buff[1];
     if (ch< (mySepar.getchannel()*2) && gr <=10 && dl<=150 && gr>0 && dl>0){
       if (state==WAIT_C);
       else error=N_EX;     
      }
      else error=N_DATE; 
    } 
    else error=N_D_KM; 
         
    myServer.out_buff[0]=error;
    myServer.set_send(1);
    myServer.sendbuff();
    if (error!=0) return;
     mySepar.testim(ch,gr,dl);
    state=TEST; 
    //cout << "testim  ch=" << ch <<" gr="<<gr<<" dl="<<dl<< endl; 
}
void CtrlSep::stoptest()
{
  error=0;
 //cout << "     Stop test" << endl; 
 //fprintf(out,"     Stop test\n");
 info="     Stop test";
 if (state==TEST){
  //stop
   state=WAIT_C;
 }
 else error=N_EX; 
 myServer.out_buff[0]=error;
 myServer.set_send(1);
 myServer.sendbuff();
 if (error==0) mySepar.stop_im(); 
}
void CtrlSep::setrgu2()
{
 unsigned short rgu;
 error=0;
 //cout << "     Setrgu2" << endl;
 // fprintf(out,"     Setrgu2\n");    
 info="     Setrgu2";
    if (myServer.get_recive()==2){
     rgu=myServer.in_buff[1];
     if (rgu==0 || rgu==1 || rgu==2){
       if (state==WAIT_C || (state==RGU && rgu==0));
       else error=N_EX;     
      }
      else error=N_DATE; 
    } 
    else error=N_D_KM; 
         
    myServer.out_buff[0]=error;
    myServer.set_send(1);
    myServer.sendbuff();
    if (error!=0) return;
    mySepar.stop_puw();
    state=RGU;
    mySepar.set_rgu2(rgu);
    if (rgu==0) state=WAIT_C;
    //cout << "setrgu2  =" << rgu << endl; 
}
void CtrlSep::getrgu2()
{
 unsigned short rgu;
 error=0;
 //cout << "     Getrgu2" << endl;
 //fprintf(out,"     Getrgu2\n");
 info="     Getrgu2";
    if (myServer.get_recive()==1){
    } 
    else error=N_D_KM; 
         
    myServer.out_buff[0]=error;
    if (error!=0) {
      myServer.set_send(1);
      myServer.sendbuff();
      return;
    }  
    rgu=mySepar.get_rgu2();
    myServer.out_buff[1]=rgu;
    myServer.set_send(2);
    myServer.sendbuff();
    if (rgu==1 || rgu==2) state=WAIT_C;
    //cout << "getrgu2  =" << rgu << endl; 
}
void CtrlSep::getblk()
{
 //cout <<"     Getblk" << endl;
 //fprintf(out,"     Getblk\n");
 info="     Getblk";
 int sz=2;
 
 myServer.out_buff[0]=error;
 myServer.out_buff[1]=(char)mySepar.getblk();
 myServer.set_send(sz);
 myServer.sendbuff();
}

void CtrlSep::initada()
{
  error=0;
 // cout << "INIT ADA" << endl; 
 if (state==WAIT_C){
  
 }
 else error=N_EX; 
 myServer.out_buff[0]=error;
 myServer.set_send(1);
 myServer.sendbuff();
 if (error==0) mySepar.initada();
}
void CtrlSep::setlog()
{
 info="     Setlog";
 int sz=1;
 if (myServer.in_buff[1]==1) f_log=true;
   else f_log=false;
 myServer.out_buff[0]=error;
 myServer.set_send(sz);
 myServer.sendbuff();
}

void CtrlSep::setim()
{
 double tt;
 error=0;
 cout << "     Setim" << endl;
 //fprintf(out,"     Testtim\n");
 info="     Setim";
    int sz=8*3*2;
    if (myServer.get_recive()==sz+1){
      //memcpy(&tt,&myServer.in_buff[1],8);
      //cout<<" tt"<<tt<<endl;
      memcpy(&mySepar.im_work,&myServer.in_buff[1],sz);
       
      //ch=myServer.in_buff[1];
     //if (ch< (mySepar.getchannel()*2) && gr <=10 && dl<=150 && gr>0 && dl>0){
       if (state==WAIT_C);
       else error=N_EX;     
      //}
      //else error=N_DATE; 
    } 
    else error=N_D_KM; 
         
    myServer.out_buff[0]=error;
    myServer.set_send(1);
    myServer.sendbuff();
    if (error!=0) return;
    mySepar.set_im();
    //state=TEST; 
    //cout << "testim  ch=" << ch <<" gr="<<gr<<" dl="<<dl<< endl; 
}
void CtrlSep::getver()
{
 cout << "     Getver"  << endl;
  //fprintf(out,"     Gettemp\n");
 info="     Getver";
 double v0,v1,v2;
 error=0;
 int sz=1;
 if (error==0) {		       
  v0=1;v1=2;v2=2;
						     
  memcpy(&myServer.out_buff[1],&v0,8);
  memcpy(&myServer.out_buff[9],&v1,8);
  memcpy(&myServer.out_buff[17],&v2,8);
  sz+=8*3;
  }     
   myServer.out_buff[0]=error;
   myServer.set_send(sz);
   myServer.sendbuff();
}
