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
#ifndef SEPAR_H
#define SEPAR_H
#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include <math.h>
#include <sys/socket.h>
#include <unistd.h>
#include "sdevice.h"
/**
@author and
*/

const bool TESTPL = false;
const int SIZE_SPK = 512;
const int MAX_OBL = 6;
const int MAX_CH  = 4;
const int MIN_TIME= 1;
const int MAX_TIME= 600;
const int S_SPK   =256;
const int BF_IM   =10;
const int MN_GZ   =4;
const int MAX_GR  =5;
const int MAX_GIST=200;
const int KC_TIME=5;
const int MAX_KC=250*KC_TIME;
const double CON_USTPUW = 18307;
const int TIMEPL = 250*120;
const int U_SIZE = 250;
const int DELAY_RGU = 30;
const int ROW = 2;
struct sobl{
 //unsigned short 
 double ls;
 //unsigned short 
 double rs;
}; 
struct sep_ust{
 sobl obl[MAX_CH][MAX_OBL];
 double gmz[MAX_CH];
 double sh1[6];
 double sh2[6];
 double sh3[6];
 double kh1[2];
 double kh2[2];
 double kh3[2];
 double prg[MAX_CH][6];
 double prg2[MAX_CH][6];
 double tiz[MAX_GR];
 double fh12;
 double fotb;
 double fotbR2;
 double maxg1;
 double ming1;
 double gcol;
 
 double  kruch;
 double  usl[MAX_CH];
 double  totb;
 double  totbR2;
 double  k_im[2][MAX_CH];
 double  b_im[2][MAX_CH];
 
 double  k_zd[2][MAX_CH];
 double  b_zd[2][MAX_CH];
 double  kprMin;
 double  kprMax;
 double  alg;
 double  sep_row;
}; 
struct ssep_work {
  //double i_kn[MAX_CH+1];
  //double i_xw[MAX_CH+1];
  //double i_km[MAX_CH+1];
  double i_prd[MAX_CH][4];
  //double p_cr[MAX_CH];
  //double p_crk[MAX_CH];
  //double p_crx[MAX_CH];
  double p_prd[MAX_CH][4];
  double p_tk[MAX_CH];
  double p_tkh1[MAX_CH];
  double p_tkh2[MAX_CH];
  double p_tkh3[MAX_CH];
  double wcount[MAX_CH];
  double s_rst[MAX_CH][MAX_GR];
  double error ;
};
struct sspk{
      unsigned short spk[S_SPK];
      };
struct sgist{
       int gist[MAX_GIST];
     };
     
struct sim_work{
  double  dl[ROW];
  double  wt[ROW];
  double  im[ROW];
};     
class Separ{
public:
     int sock;
     sep_ust *wust;
     
     ssep_work sep_work;
     sspk  *ch[MAX_CH];
     sspk  *kch[MAX_CH];
     sgist *gch[MAX_CH];
     sim_work im_work;
     int timekspk[MAX_CH];
     unsigned short kod[MAX_CH];
     int kw1,kw2,mka1,mka2;
     double loterm,hiterm,tterm;
          bool f_Reset;
     Separ();
    ~Separ();
     void setspk(int tm);
     bool flag_spk();
     void setudeu();
     void setptdeu();
     void startsep();
     void stopsep();
     int  setm50(int num,int km);
     int  getm50(int num);
     int  getchannel();
     int  getmintime();
     int  getmaxtime();
     int  getszspk();
     bool test_ust();
     void clrkspk(int ch);
     int  getszgist();
     void getren();
     void setren(int ch,int kw,int mka);
     void on_offsep(bool f);
     void on_offpren(bool f);
     void on_offexp(bool f,int ch);
     void on_offiw(bool f);
     void on_offosw(bool f);
     void setterm(int l,int h);
     void start_puw();
     void stop_puw();
     void set_puw(int ust);
     void set_rgu(int rgu); 
     void set_rgu2(int rgu);
     int  get_rgu2();
     void testim(int ch,int gr,int dl);
     void stop_im();
     void reset();
     int getblk();
     void initada();
     void set_stopall();
     void set_im();
     
     int  sizeUst();
protected:
    sep_ust   *ust;
    SDevice   mySDevice;
    pthread_t thread_id;
    pthread_cond_t flag_cv;
    pthread_mutex_t flag_mutex;
    pthread_mutex_t flag_mutex_wait;
    struct sigaction sa;
    struct itimerval timer;
    struct timeval tv,atv;
    int    time,atime;
    int    rez;
    int    max_time;
    bool   f_spk;
    int    count;
    int    time_end;
    bool   f_kon;
    bool   f_kon2;
    int    countPl;
    double kpdl;
    double bpdl;

    //GMZ
    bool f_sep;
    int intgm[MAX_CH];
    int sumgm[MAX_CH][MAX_OBL];
    bool fgm_io[MAX_CH];
    bool f_rech[MAX_CH];
    int prgm[MAX_CH];
    int ngm[MAX_CH];
    int lgm[MAX_CH];
    int countgm[MAX_CH];
    sobl oblgm[MAX_CH][MAX_OBL];
    double h1sum[MAX_CH],h2sum[MAX_CH];
    int fl_time[MAX_CH][BF_IM];
    int cn_time[MAX_CH][BF_IM];
    int dl_1[MAX_CH][BF_IM];
    int dl_2[MAX_CH][BF_IM];
    int r2_fl_time[MAX_CH][BF_IM];
    int r2_cn_time[MAX_CH][BF_IM];
    int r2_dl_1[MAX_CH][BF_IM];
    int r2_dl_2[MAX_CH][BF_IM];
    int t_ind[MAX_CH];
    static void*  execute(void* unused);
    void real_execute();
    static void timer_handler(int signum);
    void  getspk();
    
    bool f_sm50;
    bool f_gm50;
    int  m50_num,m50_km;
    int musl[MAX_CH];
    int wrm[MAX_CH];
    int kms[MAX_CH];
    int akms[MAX_CH];
    int countkc[MAX_CH];
    bool botb[MAX_CH];
    //PUW
    int ustpuw,tkustpuw;
    bool f_puw;
    int count_u;
    double u_cet;
    double setust;
    //TEST_IM
    int im_ch;
    int im_d1;
    int im_d2;
    int count_im;
    bool f_im;
    bool f_stop;
    //GMZ
    void my_gmz();
    void gm_in(int ch);
    void gm_out(int ch);
    void rec_h(int ch);
    void rec_im(int ch);
    void work_im(int ch);
    //PUW
    void work_puw();
    void work_im();
    //RGU
    bool f_rgu;
    int  m_rgu;
    bool f_stop_rgu;
    int  count_rgu;
    void work_rgu();
    // PL
    void stopAll();
    //ROW 2
    void sum_h_i(int ch,int n);
    void alg0(int ch,double h1,double h2,double h3);
    void alg1(int ch,double h1,double h2,double h3);
    void alg2(int ch,double h1,double h2,double h3);
    void alg3(int ch,double h1,double h2,double h3);
    //
    bool f_wim;
    
};

#endif
