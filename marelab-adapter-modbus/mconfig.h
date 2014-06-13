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

#ifndef MCONFIG_H_
#define MCONFIG_H_

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
#include <string>
namespace std
{

   extern string  loggmsg ;				// Central Log message string that is send to the client

   static string i2str(int i) {
   		std::ostringstream buffer;
   		buffer << i;
   		return buffer.str();
   }



   static string encodeForJSON( const string &sSrc )
   {
       ostringstream sRet;

       for( string::const_iterator iter = sSrc.begin(); iter!=sSrc.end(); iter++ )
       {
            unsigned char c = (unsigned char)*iter;

            switch( c )
            {
                case '"':
                	sRet << "'";
                	break;

                case '{':
                     sRet << "-";
                     break;
                case '}':
                     sRet << "-";
                     break;
                case '[':
                     sRet << "-";
                     break;
                case ']':
                     sRet << "-";
                     break;
                case '\r':
                     sRet << "<br id='loginfo'>";
                     break;
                case '\n':
                     sRet << "<br id='loginfo'>";
                     break;
                default:
                    sRet << c;
                    break;
            }
       }
       return sRet.str();
   }

}

#endif /* MCONFIG_H_ */
