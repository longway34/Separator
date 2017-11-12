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
#include "server.h"
using namespace std;
//static bool f_2;
Server::Server()
{
 cout << "Hello, Server!" << endl;
 //f_2=false;
 my_port=MPORT;
 memset(&my_addr,0,sizeof(my_addr));
 //listener=socket(AF_INET,SOCK_STREAM,0);
 listener = -1;
 
}


Server::~Server()
{
 close(listener);
}




/*!
    \fn Server::SetSocket()
 */
bool Server::SetSocket()
{
    /// @todo implement me
  
  close(listener);    
  listener=socket(AF_INET,SOCK_STREAM,0);
  if (listener<0){
    perror("socket");
    return false;
   }
  my_addr.sin_family=AF_INET;
  my_addr.sin_port=htons(my_port);
  my_addr.sin_addr.s_addr=htonl(INADDR_ANY);
  if (bind(listener,(struct sockaddr *)&my_addr,sizeof(my_addr))<0){
   perror("bind");
   return false;
  } 
  listen(listener,1);
  //ufds[0].fd=listener;
  //ufds[0].events=POLLIN; 
  return true;  
  }


/*!
    \fn Server::accept()
 */
bool Server::my_accept()
{
    /// @todo implement me
   my_socket=accept(listener,NULL,NULL);
   if (my_socket<0){
     perror("accept");
     return false;
   }
   return true;
}


/*!
    \fn Server::getbuff()
 */
bool Server::getbuff()
{    
    int n;
    byte_recive=0;
    //if (f_2==true){ 
     FD_ZERO(&fds);
     FD_SET(my_socket,&fds);
     tv.tv_sec=TIMEOUT;
     tv.tv_usec=0;
     n=select(my_socket+1,&fds,NULL,NULL,&tv);
     //cout<<"Select= "<<n<<endl;
     if ((n==0) || (n==-1)) return false;
    //} 
    memset(in_buff,sizeof(in_buff),0);
    byte_recive=recv(my_socket,in_buff,MAX_BUFF,0);
    if (byte_recive<=0) return false;
    else {
     //cout << " test " << byte_recive << endl;
     //f_2=true;
     return true;
    }
    
}


/*!
    \fn Server::sendbuff()
 */
bool Server::sendbuff()
{
    /// @todo implement me
    int t=send(my_socket,out_buff,byte_send,0);
    //cout << " byte_send " << byte_send << endl;
    
}


/*!
    \fn Server::get_recive()
 */
int Server::get_recive()
{
    /// @todo implement me
  return byte_recive;
}


/*!
    \fn Server::set_send(int send_byte)
 */
void Server::set_send(int send_byte)
{
    /// @todo implement me
   byte_send=send_byte;
}
