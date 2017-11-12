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
#ifndef SERVER_H
#define SERVER_H
#include <iostream>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
//#include <sys/poll.h>
/**
@author and
*/
const int MPORT = 3425;
const int MAX_BUFF = 2048;
const int TIMEOUT  = 120;
class Server{
public:
    Server();

    ~Server();
    bool SetSocket();
    bool my_accept();
    bool getbuff();
    bool sendbuff();
    int get_recive();
    void set_send(int send_byte);
    
    char in_buff[MAX_BUFF];
    char out_buff[MAX_BUFF];  
    int listener;
    int byte_send;
protected:
    sockaddr_in my_addr;
    
    int my_socket;
    int my_port;
    //char in_buff[MAX_BUFF];
    //char out_buf[MAX_BUFF];
    int byte_recive;
    
    fd_set fds;
    struct timeval tv;
    //struct pollfd ufds[1];
    
};

#endif
