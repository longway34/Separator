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
#ifndef CTRLSEP_H
#define CTRLSEP_H
#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include "ctrlsep.h"
#include "separ.h"
#include "server.h"
/**
@author and
*/
// STATE
const int WAIT_C = 0;
const int SPK    = 1;   
const int SEP    = 2;
const int RENT   = 3;
const int TERM   = 4;
const int TEST   = 5;
const int RGU    = 6;
// ERROR
const int OK     = 0;
const int N_KM   = 1;
const int N_D_KM = 2;
const int N_DATE = 3;
const int N_EX   = 4;
const int NN_EX  = 5;
const int ER_SER = 6;
const int ER_RENT= 7;
const int ER_TERM= 8;
const int ER_IM  = 9;
class CtrlSep{
public:
    CtrlSep();

    ~CtrlSep();
    void RunSep();

protected:
    Server myServer;
    Separ mySepar;
    //char* bf_in;
    //char* bf_out;
private:
    bool  f_log;
    char* info; 
    int  error;
    int  state; 
    bool f_sepon;
    bool f_sepwon;
    void sep_kom(); 
    void setspk();
    void getspk();
    void setsepar();
    void getsepar();
    void setudeu();
    void setptdeu();
    void startsep();
    void stopsep();
    void setm50();
    void getm50();
    void getstate();
    void getkspk();
    void getgist();
    void startpuw();
    void setpuw();
    void stoppuw();
    void onren();
    void offren();
    void onsep();
    void offsep();
    void oniw();
    void offiw();
    void onosw();
    void offosw();
    void setren();
    void getren();
    void settemp();
    void gettemp();
    void setrgu();
    void setrgu2();
    void getrgu2();
    void expon();
    void expoff();
    void testim();
    void stoptest();
    void getblk();
    void setlog();
    void initada();
    void setim();
    void getver();
    void no_km();
};

#endif
