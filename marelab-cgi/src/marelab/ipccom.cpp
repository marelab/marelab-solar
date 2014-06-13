/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Author      : Marc Philipp Hammermann
 * Version     :
 * Copyright © 2013 marc philipp hammermann  <marchammermann@googlemail.com>
 *
 *
 *
 *
 */
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <syslog.h>

#include "ipccom.h"
//#include "marelabconf-pi.h"
#include "marelabconf.h"


namespace std {


/*
 * Setup the Communictaion ipc socket
 */
ipccom::ipccom() {
}
void ipccom::closeServer(){
	if (socketid != -1){
		close(socketid);
		syslog( LOG_ERR, "Socket Client closed (ID:%i)",socketid);
	}
}
void ipccom::openServer() {
	 int len;
	 struct sockaddr_un local;

	 if ((socketid = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
	 {
	   throw "Socket can't be opened";
	 }
	 else
	 {
		 local.sun_family = AF_UNIX;
		 strcpy(local.sun_path, SOCK_PATH);
		 unlink(local.sun_path);
		 len = strlen(local.sun_path) + sizeof(local.sun_family);

		 if (bind(socketid, (struct sockaddr *)&local, len) == -1) {
			 throw "Socket can't be binded";
		 }

		 if (listen(socketid, 5) == -1) {
			 throw "Socket can't be listed";
		 }
		 syslog( LOG_ERR, "Server socket successfully created...");

	 }
}

void ipccom::openClient() {

	 int  len;

	 if ((socketid = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
	 {
	   throw "Socket can't be opened";
	 }
	 else
	 {
		 remote.sun_family = AF_UNIX;
		 strcpy(remote.sun_path, SOCK_PATH);
		 len = strlen(remote.sun_path) + sizeof(remote.sun_family);
		 if (connect(socketid, (struct sockaddr *)&remote, len) == -1) {
			 string error =  strerror(errno);
			 syslog( LOG_ERR, "CGI Connect 2 Nuleus Error [%s] ERROR...",error.c_str());

		 }
	 }
}


bool ipccom::recvSock(){
	int  n;
	socklen_t t;

    t = sizeof(remote);
    if ((socketret = accept(socketid, (struct sockaddr *)&remote,&t)) == -1) {
    	string error =  strerror(errno);
    	syslog( LOG_ERR, "CGI recv from Nuleus Error [%s] ERROR...",error.c_str());
    	return false;
    }
    else
    {
    	syslog( LOG_ERR, "CGI recv from Nuleus Connection accepted...");
    	/* Connected lets get something to recv*/
    	memset( sockbuf,0, TRANSFER_BUFFER);
        n = recv(socketret, sockbuf, TRANSFER_BUFFER, 0);
        if (n <= 0)
        {
           if (n < 0) {
        	   syslog( LOG_ERR, "Socket recv error...");
        	   close(socketret);
           }
           return false;
        }
        return true;
    }
    return false;
}




bool ipccom::recvSockFromNucleus(){
	int  n;
    memset( sockbuf,0, TRANSFER_BUFFER);
    syslog( LOG_ERR, "CGI waits for Nuleus answers ...");
    n = recv(socketid, sockbuf, TRANSFER_BUFFER, 0);
    if (n <= 0)
    {
      if (n < 0)
      {
    	  string error =  strerror(errno);
    	  syslog( LOG_ERR, "CGI recv from Nuleus Error [%s] ERROR...",error.c_str());
      }
      // Return was 0 so normal Soocket shutdown is okay
      return true;
    }
    syslog( LOG_ERR, "CGI recv Nuleus values  [%s] ...",getMsg().c_str());
    return true;
}

string ipccom::getMsg()
{
	string msg;
	msg = "";
	msg = sockbuf;
	return msg;
}

bool ipccom::sendSock(string bytesToSend){
    if (send(socketid, bytesToSend.c_str(), strlen(bytesToSend.c_str()), 0) == -1)
    {
        return false;
    }
    return true;
}
bool ipccom::sendSockServer(string bytesToSend){
    if (send(socketret, bytesToSend.c_str(), strlen(bytesToSend.c_str()), 0) == -1)
    {
        return false;
    }
    return true;
}

ipccom::~ipccom() {
	close(socketret);
	close(socketid);
}

} /* namespace std */
