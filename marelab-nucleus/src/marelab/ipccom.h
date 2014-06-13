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
#include <string>
#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include <sys/un.h>
#include "ConfigNucleus.h"

#ifndef IPCCOM_H_
#define IPCCOM_H_

#define TRANSFER_BUFFER 9192

namespace std {

class ipccom {
private:
	struct sockaddr_un local;
	struct sockaddr_un remote;
	int socketid;
	int socketret;
	char sockbuf[TRANSFER_BUFFER];
	ConfigNucleus *configMarelab;
	string rcvString;


public:
	ipccom(ConfigNucleus *configMarelab);
	void openServer();
	//void openClient();
	void closeServer();

	bool recvSock();
	bool recvSockClient();
	bool send2cgi(string bytesToSend);
	string getMsg();

	virtual ~ipccom();
};

} /* namespace std */
#endif /* IPCCOM_H_ */
