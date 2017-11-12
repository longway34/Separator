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
#include "separ.h"
using namespace std;
Separ::Separ()
{
 cout << "Hello, separ" << endl;
 for(int i=0;i<MAX_CH;i++) {
  ch[i]=new sspk;
  kch[i]=new sspk;
  gch[i]=new sgist;
  memset(kch[i],0,sizeof(*kch[i]));
  f_wim=false;
 } 
 
 wust=new sep_ust;
 ust=new sep_ust;
 cout<<" TTTT sizeof= " << sizeof(*wust)<<" " <<sizeof(sep_ust)<< endl;
 memset(&sep_work,0,sizeof(sep_work));
 f_spk=false;
 f_sep=false;
 f_sm50=false;
 f_gm50=false;
 f_puw=false;
 f_im=false;
 f_rgu=false;
 f_Reset=false;
 f_stop=false;
 f_stop_rgu=false;
 count_rgu=0;
 countPl=0;
 
// pthread_create(&thread_id,NULL,&execute,this);
 
 pthread_mutex_init(&flag_mutex,NULL);
 pthread_mutex_init(&flag_mutex_wait,NULL);
 pthread_cond_init(&flag_cv,NULL);
 /*
 memset(&sa,0,sizeof(sa));
 sa.sa_handler=&timer_handler;
 sigaction(SIGALRM,&sa,NULL);
 timer.it_value.tv_sec=0;
 timer.it_value.tv_usec=250000;
 timer.it_interval.tv_sec=0;
 timer.it_interval.tv_usec=3000;
 setitimer(ITIMER_REAL,&timer,NULL); */
 on_offosw(true);
}


Separ::~Separ()
{
}
void* Separ::execute(void* unused)
{
 /*
 struct sigaction sa;
 struct itimerval timer;
 
 while ( true)
  fputc('x',stderr);
 memset(&sa,0,sizeof(sa));
 sa.sa_handler=&timer_handler;
 sigaction(SIGALRM,&sa,NULL);
 timer.it_value.tv_sec=0;
 timer.it_value.tv_usec=250000;
 timer.it_interval.tv_sec=0;
 timer.it_interval.tv_usec=3000;
 setitimer(ITIMER_REAL,&timer,NULL);  
 
 getspk();
 while (true);*/
 (static_cast<Separ*>(unused))->real_execute();
 return NULL; 
}
void Separ::real_execute()
{
  memset(&sa,0,sizeof(sa));
  sa.sa_handler=&timer_handler;
  sigaction(SIGALRM,&sa,NULL);
  timer.it_value.tv_sec=0;
  timer.it_value.tv_usec=3000;
  timer.it_interval.tv_sec=0;
  timer.it_interval.tv_usec=3000; //3000
  setitimer(ITIMER_REAL,&timer,NULL);  
  
  gettimeofday(&tv,NULL);
  atime=tv.tv_sec*1000000+tv.tv_usec;
  rez=0;
  max_time=0;
  int rep =0;
  while(true) {
   pause();
   //cout <<" KC= "<<kms[2];
   
   /*
   double d=mySDevice.get_im();
   //cout<<" im_error ="<<(int)d<<endl;
   if (d>0) {
    sep_work.error=d;
    on_offiw(false);
    stop_puw();   
    //cout<<" im_error ="<<(int)d<<endl;
  
   } */   
   if (TESTPL==false){
    
    if (f_Reset==false) {
       //cout<<" c="<<countPl<<"   "<<TIMEPL<<endl;
       countPl++;
       if (countPl==TIMEPL) {
         countPl=0;
	 f_Reset=true;
	 stopAll();
       }
     }
   }
   mySDevice.getbuff();
   gettimeofday(&tv,NULL);
   time=tv.tv_sec*1000000+tv.tv_usec;
   rez=time-atime;
   atime=time;
   //cout << "time=" << rez << endl;

   if (rez>max_time) max_time=rez;
   if (rep++==5000) { //5000
     //cout <<" time = " << max_time <<" rez "<<rez<< endl;
     max_time=0;
     rep=0;
   }     
  
  pthread_mutex_lock(&flag_mutex);
  if (f_stop){
    f_stop=false;
    stopAll();
  }
  if (f_spk) {
   getspk();
   if (count++==time_end) {
    f_spk=false;
    //cout << "end  count=" << time_end << endl;
   }    
  }
  if (f_sep) {
   my_gmz();
   //cout <<" KC= "<<kms[2];
  }
  if (f_sm50){
    mySDevice.setm50(m50_num,m50_km);
    f_sm50=false;
  } 
  if (f_puw){
    work_puw();
  }
  if (f_im) work_im();
  if (f_rgu==true) work_rgu();
  /*
  if (f_gm50){
    mySDevice.getm50(m50_num);
    f_gm50=false;
  }*/
  pthread_mutex_unlock(&flag_mutex);
    
   
 } 
}
void Separ::timer_handler(int signum)
{
 /*
 static int count = 0;
 ++count;
 cout <<" time = " << count << endl;
 fputc('x',stderr);*/
}
bool Separ::flag_spk()
{  
 //cout << " f_spk " << f_spk << endl;
 if (!f_spk) return true;
 else return false;
} 
void Separ::getspk()
{
  if (~f_spk) {
   for (int i=0;i<mySDevice.sizebuff;i++){
    int d=mySDevice.buff[i];
    int ii=(d>>12) & 3;
    d&=0x0fff;
    d>>=2;
    if (d>0 && d<256) {
      if (ch[ii]->spk[d]<0xffff)
       ch[ii]->spk[d]++;//ii
      //cout << " d = "  << d << " i = "<< ii << endl;
    }
   } 
  }
}
void Separ::setspk(int tm)
{
  pthread_mutex_lock(&flag_mutex);
  //memset(*ch,0,sizeof(sspk)*4);
  for (int i=0;i<MAX_CH;i++){
   //for (int j=0;j<256;j++)
    memset(&ch[i]->spk[0],0,sizeof(sspk));
    // ch[i]->spk[j]=0;    
  }
  time_end=tm*25;
  count=0;
  f_spk=true;
  pthread_mutex_unlock(&flag_mutex);
  cout << " separ setspk " << time_end << endl;

}
void Separ::setudeu()
{
 cout<<" separ setudeu" << endl;
 for (int i=0;i<MAX_CH;i++) mySDevice.kod[i]=kod[i];
 mySDevice.setudeu();
}
void Separ::setptdeu()
{
 for (int i=0;i<MAX_CH;i++) mySDevice.kod[i]=kod[i];
 mySDevice.setptdeu();
}
void Separ::startsep()
{
  pthread_mutex_lock(&flag_mutex);
  
  
  f_sep=true;
  for (int i=0;i<MAX_CH;i++){
   for(int j=0;j<MAX_OBL;j++){
    oblgm[i][j].ls=(int)ust->obl[i][j].ls;
    oblgm[i][j].rs=(int)ust->obl[i][j].rs;
    sumgm[i][j]=0;
   }
   countkc[i]=0;
   kms[i]=0;
   akms[i]=0;
   botb[i]=false;
   cout<<" kms="<<kms[i]<<endl;
   
   prgm[i]=(int)ust->gmz[i];
   fgm_io[i]=true;
   f_rech[i]=false;
   intgm[i]=0;
   ngm[i]=0;
   lgm[i]=0;
   countgm[i]=0;
   /*
   sep_work.i_km[i]=0;
   sep_work.i_kn[i]=0;
   sep_work.i_xw[i]=0;
   sep_work.p_cr[i]=0;
   sep_work.p_crk[i]=0;
   sep_work.p_crx[i]=0;*/
   for (int j=0;j<4;j++){
    sep_work.i_prd[i][j]=0;
    sep_work.p_prd[i][j]=0;
   }
   sep_work.p_tk[i]=0;
   sep_work.wcount[i]=0;
   h1sum[i]=0;
   h2sum[i]=0;
   
   timekspk[i]=0;
   memset(kch[i],0,sizeof(*kch[i]));
   memset(gch[i],0,sizeof(*gch[i]));
   t_ind[i]=0;
   for (int j=0;j<MAX_GR;j++)
    sep_work.s_rst[i][j]=0;
   for (int j=0;j<BF_IM;j++){
    fl_time[i][j]=false;
    cn_time[i][j]=0;
    dl_1[i][j]=0;
    dl_2[i][j]=0;
    
    r2_fl_time[i][j]=false;
    r2_cn_time[i][j]=0;
    r2_dl_1[i][j]=0;
    r2_dl_2[i][j]=0;
   }
   
   // im_off(i);
  }
  pthread_mutex_unlock(&flag_mutex);
}  
void Separ::stopsep()
{
  pthread_mutex_lock(&flag_mutex);
  f_sep=false;
  f_wim=false;
  pthread_mutex_unlock(&flag_mutex);
}  
void Separ::my_gmz()
{
 //cout << " GMZ " << endl;
 for (int i=0;i<mySDevice.sizebuff;i++){
    int d=mySDevice.buff[i];
    unsigned short ii=(d>>12) & 3;
    d&=0x0fff;
    d=musl[ii]*d>>16;
    d>>=2;
    //cout <<   "    d = "  << d << " i = "<< ii << endl;
    if (d>0 && d<256 && ii<MAX_CH) {
       //cout << "usl  d = "  << d << " i = "<< ii << endl;
       if (fgm_io[ii]==true) {
        if (kch[ii]->spk[d]<0xffff)
	 kch[ii]->spk[d]++;
       }
       intgm[ii]++;
       for (int j=0;j<MAX_OBL;j++)
         if( d>=oblgm[ii][j].ls && d<=oblgm[ii][j].rs) sumgm[ii][j]++;
    }
 }
 for (int i=0;i<MAX_CH;i++){
  //cout << "  intg " << intgm[i] << "  fgm " <<fgm_io[i]<< endl;
  work_im(i);
  if (fgm_io[i]==true){ 
    timekspk[i]++;
    gm_in(i);
   }    
   else gm_out(i);
   intgm[i]=0;
  if (f_rech[i]==true){
    //cout << " f_rech " << countgm[i] << endl;
    f_rech[i]=false;
    rec_im(i);
    //rec_h(i);
  } 
 } 
}
void Separ::work_im(int ch)
{
if(++countkc[ch]>MAX_KC){
   countkc[ch]=0;
   //cout <<" kms="<< kms[ch]<<endl;
   double k=(kms[ch])/KC_TIME;
   if (k>=ust->totb) botb[ch]=true;
               else  botb[ch]=false;
   	       
   //cout <<"ch="<<ch<<" totb="<< ust->totb<<" KC= "<<k<<"  botb="<<botb[ch]<<" kms="<< kms[ch]<<endl;	            
   kms[ch]=0;
 }
 for (int i=0;i<BF_IM;i++){ 
  if (fl_time[ch][i]==true){
   cn_time[ch][i]++;
   if(cn_time[ch][i]==dl_1[ch][i]) {
   if (botb[ch]==false){
      mySDevice.set_im(ch,true);
    //IM_ON
    //gettimeofday(&tv,NULL);
    //time=tv.tv_sec*1000000+tv.tv_usec;
    //cout << "IM_ON "<<endl;
    //time/1000<<endl;
    //kms[ch]++;
    }
   }
   if(cn_time[ch][i]>=dl_2[ch][i]){
      mySDevice.set_im(ch,false);
    //IM_OFF
    //gettimeofday(&tv,NULL);
    //time=tv.tv_sec*1000000+tv.tv_usec;
    fl_time[ch][i]=false;
    cn_time[ch][i]=0;
    //cout << "IM_OFF "<<time/1000<<endl;
   }
  }
  
 // ROW 2  
  
  if (r2_fl_time[ch][i]==true){
   r2_cn_time[ch][i]++;
   if(r2_cn_time[ch][i]==r2_dl_1[ch][i]) {
   
      mySDevice.set_im(ch+4,true);
    //IM_ON
    //gettimeofday(&tv,NULL);
    //time=tv.tv_sec*1000000+tv.tv_usec;
    //cout << "IM_ON "<<endl;
    //time/1000<<endl;
    //kms[ch]++;
    
   }
   if(r2_cn_time[ch][i]>=r2_dl_2[ch][i]){
      mySDevice.set_im(ch+4,false);
    //IM_OFF
    //gettimeofday(&tv,NULL);
    //time=tv.tv_sec*1000000+tv.tv_usec;
    r2_fl_time[ch][i]=false;
    r2_cn_time[ch][i]=0;
    //cout << "IM_OFF "<<time/1000<<endl;
   }
  }
 
 
 } 
}
void Separ::gm_in(int ch)
{
 //cout << "gm_in prg " << prgm[ch] << endl;
 if (intgm[ch]>prgm[ch]){
  countgm[ch]=ngm[ch]+lgm[ch];
  ngm[ch]++;
  if (ngm[ch]>1){
   fgm_io[ch]=false;
   countgm[ch]=ngm[ch]+lgm[ch];
   ngm[ch]=0;
   lgm[ch]=0;
  }
 }
 else {
  lgm[ch]++;
  if (lgm[ch]>1 || ngm[ch]==0){
   ngm[ch]=0;
   lgm[ch]=0;
   //
   for(int i=0;i<MAX_OBL;i++) sumgm[ch][i]=0;
  }
 }
}
void Separ::gm_out(int ch)
{
 //cout << "gm_out " << countgm[ch] << endl;
 countgm[ch]++;
 if (intgm[ch]<prgm[ch]){
  ngm[ch]++;
  if (ngm[ch]>1){
   ngm[ch]=0;
   lgm[ch]=0;
   fgm_io[ch]=true;
   f_rech[ch]=true;
  }
 }
 else {
  if (ngm[ch]>0) {
   lgm[ch]++;
   if (lgm[ch]>1){
    ngm[ch]=0;
    lgm[ch]=0;
   }
  } 
 }
}
void Separ::sum_h_i(int ch,int n)
 {
    sep_work.p_prd[ch][n]=sep_work.p_prd[ch][n]*(sep_work.i_prd[ch][n]/(sep_work.i_prd[ch][n]+1))+
    sep_work.p_tk[ch]/(sep_work.i_prd[ch][n]+1);
    sep_work.i_prd[ch][n]++;
 }
void Separ::alg0(int ch,double h1,double h2,double h3)
 {
 //cout<<" alg 0"<<endl;
 if((h1>ust->prg[ch][0] && h2<ust->prg[ch][1])||(h2<ust->prg[ch][2])||(h2>ust->prg[ch][3])
     ||(h3<ust->prg[ch][4])||(h3>ust->prg[ch][5])) f_kon=true;
  else f_kon=false;
  if (f_kon==true) {
    sum_h_i(ch,0);
   //cout<<" KON"<<endl;
   } else {
    sum_h_i(ch,2);
    //cout<<" XW"<<endl;
   }
   if((ust->fotb==1 && f_kon==true)||(ust->fotb==0 && f_kon==false)) {
    fl_time[ch][t_ind[ch]]=false;
   }
   else {
    fl_time[ch][t_ind[ch]]=true;    
    kms[ch]++;
    
   }
 }
void Separ::alg1(int ch,double h1,double h2,double h3)
 {
    // cout<<" alg 1"<<endl;
    //Alex 25.08.2011 Проверка условий по первому параметру
	if( ( h1 > ust->prg[ch][0] && h2 < ust->prg[ch][1] ) ||
	    ( h2<ust->prg[ch][2]) ||
	    ( h2>ust->prg[ch][3]) ||
	    ( h3<ust->prg[ch][4]) ||
	    ( h3>ust->prg[ch][5]) ) f_kon=true;
	else f_kon=false;

	if( f_kon == true )
	{
	sum_h_i(ch,0);
	//cout<<" KON"<<endl;
	}

	if( ust->fotb == 1 )	//Отдельно учитываем инверсию первого ряда
	{
		if( ( ( f_kon == false ) && ( h1 > ust->prg2[ch][0] ) ) || ( f_kon == true ) )
		{
			fl_time[ch][t_ind[ch]]=false;
		}
		else
		{
			fl_time[ch][t_ind[ch]]=true;
			kms[ch]++;
		}
	}
	else
	{
		//Alex 25.08.2011 Учёт инверсии отбора по первому ряду
		if( ( ust->fotb == 1 && f_kon == true ) || ( ust->fotb == 0 && f_kon == false ) )
		{
			fl_time[ch][t_ind[ch]]=false;
		}
		else
		{
			fl_time[ch][t_ind[ch]]=true;
			kms[ch]++;
		}
	}

    /*if( ( h1 > ust->prg[ch][0] && h2 < ust->prg[ch][1] ) ||
            ( h2<ust->prg[ch][2]) ||
            ( h2>ust->prg[ch][3]) ||
            ( h3<ust->prg[ch][4]) ||
            ( h3>ust->prg[ch][5]) ) f_kon=true;
        else f_kon=false;
    if( f_kon == true )
    {
        sum_h_i(ch,0);
        //cout<<" KON"<<endl;
    }
    //    else {
    //    sum_h_i(ch,2);
    //    cout<<" XW"<<endl;
    //   }
    //Alex 25.08.2011 Учёт инверсии отбора по первому ряду
    if( ( ust->fotb == 1 && f_kon == true ) || ( ust->fotb == 0 && f_kon == false ) )
    {
        fl_time[ch][t_ind[ch]]=false;
    }
    else
    {
        fl_time[ch][t_ind[ch]]=true;
        kms[ch]++;
    }*/

    //if (f_kon==false)
    if( ! fl_time[ ch ][ t_ind[ ch ] ] )	//Alex 25.08.2011
    {
	if( ust->fotb == 1 )	//Отдельно учитываем инверсию первого ряда
	{
		if(( ( h1 > ust->prg2[ch][0] ) && ( h1 < ust->prg[ch][0] ) && ( h2 < ust->prg2[ch][1] ) )
			||(h2<ust->prg2[ch][2])||(h2>ust->prg2[ch][3])
        		||(h3<ust->prg2[ch][4])||(h3>ust->prg2[ch][5]))
	            f_kon2=true;
        	else f_kon2=false;
	}
	else
	{
		if((h1>ust->prg2[ch][0] && h2<ust->prg2[ch][1])
			||(h2<ust->prg2[ch][2])||(h2>ust->prg2[ch][3])
        		||(h3<ust->prg2[ch][4])||(h3>ust->prg2[ch][5]))
	            f_kon2=true;
        	else f_kon2=false;
	}
	//В зависимости от типа руды и инверсии второго ряда
	if( f_kon2 )
	{
            sum_h_i(ch,1);
            r2_fl_time[ ch ][ t_ind[ ch ] ] = ust->fotbR2 == 0.0;
	}
	else
	{
            sum_h_i(ch,2);
            r2_fl_time[ ch ][ t_ind[ ch ] ] = ust->fotbR2 != 0.0;
	}
        /*if((h1>ust->prg2[ch][0] && h2<ust->prg2[ch][1])||(h2<ust->prg2[ch][2])||(h2>ust->prg2[ch][3])
        ||(h3<ust->prg2[ch][4])||(h3>ust->prg2[ch][5]))
        {
            f_kon2=true;
            sum_h_i(ch,1);
            //r2_fl_time[ch][t_ind[ch]]=true;
            r2_fl_time[ ch ][ t_ind[ ch ] ] = ust->fotbR2 == 0.0;
        }
        else
        {
            f_kon2=false;
            sum_h_i(ch,2);
            //r2_fl_time[ch][t_ind[ch]]=false;
            r2_fl_time[ ch ][ t_ind[ ch ] ] = ust->fotbR2 != 0.0;
        }*/
    }
 }
 
void Separ::alg2(int ch,double h1,double h2,double h3)
 {
 //cout<<" alg 2"<<endl;
 int cn=countgm[ch]*MN_GZ;
 bool f_row=false;
 if (cn<ust->sep_row) f_row=true;
 if((h1>ust->prg[ch][0] && h2<ust->prg[ch][1])||(h2<ust->prg[ch][2])||(h2>ust->prg[ch][3])
     ||(h3<ust->prg[ch][4])||(h3>ust->prg[ch][5])) f_kon=true;
  else f_kon=false;
  
  if (f_kon && f_row) {
    sum_h_i(ch,0);
    fl_time[ch][t_ind[ch]]=true;
  } 
  
  if (f_kon && !f_row) {
    sum_h_i(ch,1);
    r2_fl_time[ch][t_ind[ch]]=true;
  }
  
  if (!f_kon ) {
    sum_h_i(ch,2);
    fl_time[ch][t_ind[ch]]=false;
    r2_fl_time[ch][t_ind[ch]]=false;
  } 
 } 
 
void Separ::alg3(int ch,double h1,double h2,double h3)
 {
 //cout<<" alg 3"<<endl;
 int km=(int)(sep_work.i_prd[ch][0]+sep_work.i_prd[ch][1]);
 km=km & 1; 
 bool f_row=false;
 if (km==1) f_row=true;
 if((h1>ust->prg[ch][0] && h2<ust->prg[ch][1])||(h2<ust->prg[ch][2])||(h2>ust->prg[ch][3])
     ||(h3<ust->prg[ch][4])||(h3>ust->prg[ch][5])) f_kon=true;
  else f_kon=false;
  
  if (f_kon && f_row) {
    sum_h_i(ch,0);
    fl_time[ch][t_ind[ch]]=true;
  } 
  
  if (f_kon && !f_row) {
    sum_h_i(ch,1);
    r2_fl_time[ch][t_ind[ch]]=true;
  }
  
  if (!f_kon ) {
    sum_h_i(ch,2);
    fl_time[ch][t_ind[ch]]=false;
    r2_fl_time[ch][t_ind[ch]]=false;
  } 
 } 

 
void Separ::rec_h(int ch)
{
  double h1=0;
  double h2=0;
  double h3=0;
  double w=0;
  double w0=0;
  double w1=0;
  w=sumgm[ch][(int)ust->sh1[0]]+ust->kh1[0]*sumgm[ch][(int)ust->sh1[1]];
  w0=sumgm[ch][(int)ust->sh1[4]];
  if (w0!=0){
    w1=sumgm[ch][(int)ust->sh1[2]]+ust->kh1[1]*sumgm[ch][(int)ust->sh1[3]]/sumgm[ch][(int)ust->sh1[4]]*sumgm[ch][(int)ust->sh1[5]];
   if ( w1!=0 ) w=w/w1;
   else w=0;
   h1=w;
  } 
  w=sumgm[ch][(int)ust->sh2[0]]+ust->kh2[0]*sumgm[ch][(int)ust->sh2[1]];
  w0=sumgm[ch][(int)ust->sh2[4]];
  if (w0!=0){
   w1=sumgm[ch][(int)ust->sh2[2]]+ust->kh2[1]*sumgm[ch][(int)ust->sh2[3]]/sumgm[ch][(int)ust->sh2[4]]*sumgm[ch][(int)ust->sh2[5]];
  if ( w1!=0 ) w=w/w1;
  else w=0;
  h2=w; 
  }
  w=sumgm[ch][(int)ust->sh3[0]]+ust->kh3[0]*sumgm[ch][(int)ust->sh3[1]];
  w0=sumgm[ch][(int)ust->sh3[4]];
  if (w0!=0){
   w1=sumgm[ch][(int)ust->sh3[2]]+ust->kh3[1]*sumgm[ch][(int)ust->sh3[3]]/sumgm[ch][(int)ust->sh3[4]]*sumgm[ch][(int)ust->sh3[5]];
  if ( w1!=0 ) w=w/w1;
  else w=0;
  h3=w; 
  }
  
  double kfpr=kpdl*countgm[ch]*4+bpdl;
  //cout<<" h1="<<h1<<" h2=" <<h2<<" h3="<<h3<<endl;
  h1=h1*kfpr;
  h2=h2*kfpr;
  h3=h3*kfpr;
  //cout<<"kfpr="<<kfpr<<" count="<<countgm[ch]*4<<" h1="<<h1<<" h2=" <<h2<<" h3="<<h3<<endl;
  sep_work.p_tkh1[ch]=h1;
  sep_work.p_tkh2[ch]=h2;
  sep_work.p_tkh3[ch]=h3;
  if(ust->fh12==0) sep_work.p_tk[ch]=h1;
  else if(ust->fh12==1) sep_work.p_tk[ch]=h2;
          else sep_work.p_tk[ch]=h3;     
	  
 switch ((int)ust->alg){
 
  case 0:alg0(ch,h1,h2,h3);break;
  case 1:alg1(ch,h1,h2,h3);break;
  case 2:alg2(ch,h1,h2,h3);break;
  case 3:alg3(ch,h1,h2,h3);break;
 } 
//-------   
/*
  if((h1>ust->prg[ch][0] && h2<ust->prg[ch][1])||(h2<ust->prg[ch][2])||(h2>ust->prg[ch][3])
     ||(h3<ust->prg[ch][4])||(h3>ust->prg[ch][5])) f_kon=true;
  else f_kon=false;
  if (f_kon==true) {
   //sep_work.p_crk[ch]=sep_work.p_crk[ch]*(sep_work.i_kn[ch]/(sep_work.i_kn[ch]+1))+
  // sep_work.p_tk[ch]/(sep_work.i_kn[ch]+1);
  // sep_work.i_kn[ch]++;
    sum_h_i(ch,0);
   cout<<" KON"<<endl;
   } else {
  // sep_work.p_crx[ch]=sep_work.p_crx[ch]*(sep_work.i_xw[ch]/(sep_work.i_xw[ch]+1))+
  // sep_work.p_tk[ch]/(sep_work.i_xw[ch]+1);
  // sep_work.i_xw[ch]++;
    sum_h_i(ch,2);
    cout<<" XW"<<endl;
   }
   if((ust->fotb==1 && f_kon==true)||(ust->fotb==0 && f_kon==false)) {
   //if (f_kon==false){   
    fl_time[ch][t_ind[ch]]=false;
   }
   else {
    fl_time[ch][t_ind[ch]]=true;
    
    kms[ch]++;
    
   }
   
   //if (botb[ch]==true) fl_time[ch][t_ind[ch]]=false;
   
 // 2 ROW
   if (f_kon==false) {
   
    if((h1>ust->prg2[ch][0] && h2<ust->prg2[ch][1])||(h2<ust->prg2[ch][2])||(h2>ust->prg2[ch][3])
     ||(h3<ust->prg2[ch][4])||(h3>ust->prg2[ch][5])) 
     {
      f_kon2=true;
      r2_fl_time[ch][t_ind[ch]]=true;
    }  
    else
    {
     f_kon2=false;
     r2_fl_time[ch][t_ind[ch]]=false;
    }
   
   }  
*/   
//--------   
   
   t_ind[ch]++;
   if(t_ind[ch]>=BF_IM) t_ind[ch]=0;
   //cout<<"t_ind="<<t_ind[ch]<<endl;
    //time=tv.tv_sec*1000000+tv.tv_usec;
    //cout << "IM_START "<<time/1000<<endl;
  //else w=99;
  //cout << "Ob0L="<< oblgm[ch][0].ls <<" Ob0R=" << oblgm[ch][0].rs << endl;
  //cout << "Ob1L="<< oblgm[ch][1].ls <<" Ob1R=" << oblgm[ch][1].rs << endl;
  //cout <<" CH= "<< ch <<" count = "<< countgm[ch] << " H1 = " << w << " S0= " << sumgm[ch][0]<< " S1= " << sumgm[ch][1] << endl;
  for(int i=0;i<MAX_OBL;i++) sumgm[ch][i]=0;
  //h1sum[ch]+=h1;
      
  //sep_work.p_cr[ch]=sep_work.p_cr[ch]*(sep_work.i_km[ch]/(sep_work.i_km[ch]+1))+
  //sep_work.p_tk[ch]/(sep_work.i_km[ch]+1);


  
  sum_h_i(ch,3);
  //GIST
  double t=ust->maxg1-ust->ming1;
  if (t!=0) t=(sep_work.p_tk[ch]-ust->ming1)/t*ust->gcol;
  int it=(int)t;
  if ((t-it)>=0.5) it++;
  
  if(t>=0 && t<=ust->gcol) gch[ch]->gist[it]++;
                     else gch[ch]->gist[(int)ust->gcol-1]++;
  //cout<<"GIST= " << t << "  it="<<it<<endl;
  
  //sep_work.i_km[ch]++;
  //sep_work.i_prd[ch][3]++;
 

  sep_work.wcount[ch]=countgm[ch]*4;
  //cout<<" P0="<< ust.prg[ch][0] <<" P1="<<ust.prg[ch][1]<<" P2="<<ust.prg[ch][2]<<" P3="<<ust.prg[ch][3]<<endl;
 //cout<<ch<<" km="<<sep_work.i_km[ch]<<" cr="<<sep_work.p_cr[ch]<<" H1=" <<h1<<" H2=" <<h2<<
//  " kn="<<sep_work.i_kn[ch]<<" crk="<<sep_work.p_crk[ch]<<" xw="<<sep_work.i_xw[ch]<<" crx="<<sep_work.p_crx[ch]<<endl; 
// cout<<ch<<" km="<<sep_work.i_km[ch]<<" kn="<<sep_work.i_kn[ch]<<" % "<<sep_work.i_kn[ch]/sep_work.i_km[ch]*100<<endl; 
/*
 cout<<"ch="<<ch;
 for (int i=0;i<4;i++)  cout<<" "<<sep_work.i_prd[ch][i]<<" ";
 cout<<endl;
 for (int i=0;i<4;i++)  cout<<" "<<sep_work.p_prd[ch][i]<<" ";
 cout<<endl;*/
}
void Separ::rec_im(int ch)
{
 int count=countgm[ch]*MN_GZ;
 //for (int i=0;i<MAX_GR;i++) cout << " tiz= " <<ust->tiz[i] <<endl;
 
 if (count>ust->tiz[MAX_GR-1] || count<ust->tiz[0]) {
   sep_work.s_rst[ch][0]++;
   //return;
 }
 else {
 
  for(int i=1;i<MAX_GR;i++){
   if (count<ust->tiz[i]) {sep_work.s_rst[ch][i]++;break;}
  }
  double zd=count*ust->k_zd[0][ch]+ust->b_zd[0][ch];
  double im=count*ust->k_im[0][ch]+ust->b_im[0][ch];
  //cout <<"count="<<count<< " zd="<<zd<<" im="<<im<<endl;
  if (f_wim){
    if (count>im_work.dl[0]){
    zd=im_work.wt[0];
    im=im_work.im[0];
    //cout <<"1 row !!! count="<<count<< " zd="<<zd<<" im="<<im<<endl;
    }
  }
  if (zd<0 || im+zd<0) { zd=0;im=0;}
  dl_2[ch][t_ind[ch]]=(int)((zd+im)/4);
  dl_1[ch][t_ind[ch]]=(int)(zd/4);
  
  //cout <<"count="<<count<< " zd="<<zd<<" im="<<im<<" dl1="<<dl_1[ch][t_ind[ch]]<<" dl2="<<dl_2[ch][t_ind[ch]]<<endl;
  // ROW 2
  zd=count*ust->k_zd[1][ch]+ust->b_zd[1][ch];
  im=count*ust->k_im[1][ch]+ust->b_im[1][ch];
  if (f_wim){
    if (count>im_work.dl[1]){
    zd=im_work.wt[1];
    im=im_work.im[1];
    //cout <<"2 row !!! count="<<count<< " zd="<<zd<<" im="<<im<<endl;
    }
  }
  if (zd<0 || im+zd<0) { zd=0;im=0;}
  r2_dl_2[ch][t_ind[ch]]=(int)((zd+im)/4);
  r2_dl_1[ch][t_ind[ch]]=(int)(zd/4);
  rec_h(ch);
 }
 /*
 cout << "dlt="<< count << endl;
 for (int i=0;i<MAX_GR;i++) 
 cout << " s_rst= "<< sep_work.s_rst[ch][i]<<"  ";
 cout<<endl;*/
}
int Separ::setm50(int num,int km)
{
 cout << " separ::setm50 num km " <<num<<"  "<< km <<endl;
 //mySDevice.setm50(num,km);
 f_sm50=true;
 m50_num=num;
 m50_km=km;
}
int Separ::getm50(int num)
{
 cout << " separ::getm50 " << endl;
 int w=mySDevice.getm50(num);
 //f_gm50=true;
 m50_num=num;
 return w;
}
int Separ::getchannel()
{
 return MAX_CH;
}
int Separ::getmintime()
{
 return MIN_TIME;
}
int Separ::getmaxtime()
{
 return MAX_TIME;
}
int Separ::getszspk()
{
 return SIZE_SPK;
}
int Separ::getszgist()
{
 return (sizeof(sgist));
}
bool Separ::test_ust()
{
 
 for(int i=0;i<MAX_CH;i++){
  for(int j=0;j<MAX_OBL;j++){
   if(wust->obl[i][j].ls<0 || wust->obl[i][j].ls>=S_SPK || 
      wust->obl[i][j].ls>wust->obl[i][j].rs ||
      wust->obl[i][j].rs<0 || wust->obl[i][j].rs>=S_SPK) return false;
  }
  if( wust->gmz[i]<1 || wust->gmz[i]>100) return false;
  for (int j=0;j<6;j++){
   if(wust->prg[i][j]< -99 || wust->prg[i][j]> 99) return false;
  }
  //cout<<"usl== "<<wust->usl[i]<<endl;
  if (wust->usl[i]<0.5 || wust->usl[i]>2)return false;
 }
 for (int i=0;i<6;i++){
   if(wust->sh1[i]<0 || wust->sh1[i]>=MAX_OBL) return false;
   if(wust->sh2[i]<0 || wust->sh2[i]>=MAX_OBL) return false;
   if(wust->sh3[i]<0 || wust->sh3[i]>=MAX_OBL) return false;
 }
 
 if (wust->fh12==0 || wust->fh12==1 || wust->fh12==2);
 else return false;
 
 if (wust->fotb==0 || wust->fotb==1);
 else return false;
 if (wust->ming1 > wust->maxg1) return false;
 if (wust->gcol<10 || wust->gcol>MAX_GIST) return false;
 if (wust->kruch<1 || wust->kruch>4) return false;
 if (wust->totb<1 || wust->totb>10) return false;
 cout << " sizeof= "<<sizeof(*ust)<<endl;
 memcpy(&*ust,&*wust,sizeof(*ust));
 
 for (int i=0;i<MAX_CH;i++) {
  musl[i]=(int)(ust->usl[i]*0x10000);
  //cout<<"usl= "<<ust->usl[i]<<" musl= "<<musl[i]<<endl;
 }
  kpdl=0;bpdl=1;
  if ((ust->tiz[4]-ust->tiz[0])!=0) {
   kpdl=(ust->kprMax-ust->kprMin)/(ust->tiz[4]-ust->tiz[0]);
   bpdl=ust->kprMin-ust->tiz[0]*kpdl;
  } 
  cout<<"dMin "<<ust->tiz[0] <<" dMax "<<ust->tiz[4]<<endl;
  cout<<"kpdl="<<kpdl<<" bpdl="<<bpdl<<endl;

 return true;
}
void Separ::clrkspk(int ch)
{
 timekspk[ch]=0;
 memset(kch[ch],0,sizeof(*kch[ch]));
}
void Separ::getren()
{
 kw1=mySDevice.adc(4);
 kw1=mySDevice.adc(4);
 
 mka1=mySDevice.adc(5);
 mka1=mySDevice.adc(5);
 
 kw2=mySDevice.adc(6);
 kw2=mySDevice.adc(6);
 
 mka2=mySDevice.adc(7);
 mka2=mySDevice.adc(7);
}
void Separ::setren(int ch,int kw,int mka)
{
 int i=0;
 if (ch==0) i=2;
 mySDevice.setdac(i,mka);
 mySDevice.setdac(i+1,kw);
 cout<<"ch="<<ch<<" mka="<<mka<<" kw="<<kw<<endl;
}
void Separ::on_offsep(bool f)
{
 sep_work.error=0;
 mySDevice.on_offsep(f);
}
void Separ::on_offpren(bool f)
{
 mySDevice.on_offpren(f);
}
void Separ::on_offexp(bool f,int ch)
{
 mySDevice.on_offexp(f,ch);
}
void Separ::setterm(int l,int h)
{
 if (l<-55 | l>125 | h<-55 | h>125) {l=0;h=0;}
 if (l!=0 && h!=0){ 
   mySDevice.set_lht(l,h);
   cout<<" LO="<<l<<" HI="<<h<<endl;
 }
 tterm=mySDevice.read_tm();
 loterm=mySDevice.read_lt();
 hiterm=mySDevice.read_ht();
}
void Separ::start_puw()
{
 ustpuw=(int)CON_USTPUW;
 tkustpuw=(int)CON_USTPUW;
 mySDevice.start_puw(ustpuw);
 count_u=0;
 u_cet=0;
 f_puw=true;
}
void Separ::stop_puw()
{
 f_puw=false;
 mySDevice.stop_puw();
}
void Separ::set_puw(int ust)
{
 setust=ust;
}
void Separ::work_puw()
{
 double s=2;
 double pi=3.14;
 int d=mySDevice.adc(0);
 d=mySDevice.adc(0);
 u_cet+=d;
 if (++count_u==U_SIZE) {
   if (U_SIZE!=0){
    u_cet=u_cet/U_SIZE;
    //cout<<" d="<<d<<" u_cet="<<u_cet<<endl;
    //cout<<"TK_UST="<<tkustpuw<<" UST="<<ustpuw<<endl;
    double w=u_cet*sqrt(s);
    double w2=setust;
    double wzu=w;
    u_cet=0;
    count_u=0;
    if (w!=0){
     w=w2/w-1;
     if ((1-w*w)>0 && w!=0) {
      w=atan(w/sqrt(1-w*w));
      w=pi/2-w;
      w=(CON_USTPUW/pi)*w;
      if (w<CON_USTPUW &&  w>0) ustpuw=int(w);
     }
    }  
   }
  } 
  if (tkustpuw!=ustpuw) {
     if (tkustpuw>ustpuw) tkustpuw-=4;
                  else    tkustpuw+=4;
     
     mySDevice.set_puw(tkustpuw);		         
  }
}
void Separ::set_rgu(int rgu)
{
 mySDevice.set_rgu(rgu);
}
void Separ::testim(int ch,int gr,int dl)
{
 im_ch=ch;
 im_d1=(int)dl/4;
 im_d2=(int)1000/gr/4;
 if (im_d1>=im_d2) im_d1=(int)im_d1/2;
 cout<<" im_d1="<<im_d1<<" im_d2="<<im_d2<<endl;
 count_im=0;
 f_im=true;
}
void Separ::work_im()
{
 if (count_im==0) mySDevice.set_im(im_ch,true); 
 if (count_im==im_d1) mySDevice.set_im(im_ch,false);
 if (count_im==im_d2) count_im=0;
                 else count_im++;                   
 
}
void Separ::stop_im()
{
 f_im=false;
 for (int i=0;i<MAX_CH*2;i++) mySDevice.set_im(i,false); 
}
void Separ::on_offiw(bool f)
{
 mySDevice.on_offiw(f);
}
void Separ::on_offosw(bool f)
{
 mySDevice.on_offosw(f);
}
void Separ::set_rgu2(int rgu)
{
 count_rgu=0;
 f_rgu=false;
 if (rgu==0){
  f_stop_rgu=false; 
  
  m_rgu=rgu;
  mySDevice.set_rgu(m_rgu);
 }
 else{
  m_rgu=rgu;
  f_rgu=true;
 } 
}
int Separ::get_rgu2()
{
 return mySDevice.get_rgu2();
}
void Separ::work_rgu()
{
 int r=mySDevice.get_rgu2(); 
 
 if (f_stop_rgu){
    count_rgu++;
    if (count_rgu>=DELAY_RGU){
     mySDevice.set_rgu(0);
     f_rgu=false;
     f_stop_rgu=false; 
     count_rgu=0;
    } 
    cout<<"stop rgu "<<count_rgu<< endl;
    return;
  }
  
  if (r!=m_rgu) mySDevice.set_rgu(m_rgu);
 // while (1) {
    r=mySDevice.get_rgu2();
    //cout<<" RGU= "<<r<<" m_rgu="<<m_rgu<<endl;
    if (r==m_rgu) {     
      //mySDevice.set_rgu(0);
      //f_rgu=false;
      f_stop_rgu=true;
      count_rgu=0;
    }
}
void Separ::stopAll()
{
 f_spk=false;
 f_sep=false;
 f_sm50=false;
 f_gm50=false;
 f_puw=false;
 f_im=false;
 f_rgu=false;
 f_stop_rgu=false;
 count_rgu=0;
 stop_im();
 on_offosw(false);
 on_offiw(false);
 stop_puw();
 on_offexp(false,0);
 on_offexp(false,1);
 on_offpren(false);
 set_rgu2(0);
 on_offsep(false);
 //initada();
 //int t=shutdown(sock,2);
 cout<< " STOP ALL "<<endl; 
}
void Separ::reset()
{
 countPl=0;
 f_Reset=false;
 //cout<<" RESET "<<endl;
}
void Separ::initada()
{
 mySDevice.initada();
}
int Separ::getblk()
{
 return mySDevice.getblk();
}
void Separ::set_stopall()
{
 f_stop=true;
}
int Separ::sizeUst()
{
 int sz=sizeof(*wust);
 cout<<" SZ="<<sz<<endl;
 return sz;
}
void Separ::set_im()
{
 f_wim=true;
 cout<<"work_IM"<<endl;
 cout<<" dl="<<im_work.dl[0]<<" wt="<<im_work.wt[0]<<" im="<<im_work.im[0]<<endl;
 cout<<" dl="<<im_work.dl[1]<<" wt="<<im_work.wt[1]<<" im="<<im_work.im[1]<<endl;
}
