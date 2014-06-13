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

#ifndef MLOG_H_
#define MLOG_H_

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <errno.h>
#include <pwd.h>
#include <signal.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cctype>
#include <cwctype>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <time.h>
#include <stdlib.h>
#include <sstream>
#include <cstring>
#include <string>

#include "../mconfig.h"
namespace std
{
extern string  loggmsg ;				// Central Log message string that is send to the client

class MLOG{
public:


	static string getLogMsg(){
		return loggmsg;
	}

	static void log(string message, int iline,string file){
		//bool logfile = true;
		bool logconsole = true;
		bool logclient = true;


		string line;
		string datum;

		time_t t = time(0);   // get time now
		struct tm * now = localtime( & t );

		datum = i2str((now->tm_year + 1900)) + '-'
		        + i2str((now->tm_mon + 1)) + '-'
		        + i2str(now->tm_mday)+ ' '
		        + i2str(now->tm_hour)+ ':'
		        + i2str(now->tm_min) + ':'
		        + i2str(now->tm_sec);

		line = i2str(iline) + " ";



		//if (logfile){

		//	syslog( LOG_INFO, "%s",message.c_str() );

		//}

		if (logconsole){
			cout <<datum<< file << " Line:" << line << message << endl;
		}
		if (logclient){
			loggmsg = loggmsg +datum+ file + " Line:" + line + "\n" +message + "\n";
			//cout << "LOG:" << message << endl;
			if (loggmsg.size() > 4096)
				loggmsg.clear();
		}

	}
};

}
#endif /* LOG_H_ */
