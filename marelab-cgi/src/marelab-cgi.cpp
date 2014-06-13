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
 * Name        : marelab-cgi.cpp
 * Author      : Marc Philipp Hammermann
 * Version     :
 * Copyright © 2012 marc philipp hammermann  <marchammermann@googlemail.com>
 *
 * Description : cgi 2 ipc message converter. it takes cgi request and sends them
 *               to the marelab-nucleus deamon over IPC the result is returned as
 *               JSON formated message. The cgi converter is just a protocol
 *               converter with to exceptions:
 *
 *               1.) It can restart the 		  "url?COMMAND=STARTDEAMON"
 *               2.) It can give back its version "url?COMMAND=MARELAB_CGI_VERSION"
 *
 *
 */
#include <iostream>
#include <vector>
#include <string>
#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/CgiUtils.h"
#include "cgicc/CgiEnvironment.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"
#include "marelab/ipccom.h"
//#include "marelabconf-pi.h"
#include "marelab/marelabconf.h"

#include <stdio.h>
#include <stdlib.h>

/*
 * DEFINES GLOBAL
 */

/* Change this to whatever your daemon places the ipc*/
//#define DAEMON_PIPE "/home/ths/msp"

using namespace std;
using namespace cgicc;     // Or reference as cgicc::Cgicc formData; below in object instantiation.

/* CGI VERSION TAG */
#define marelab_cgi_version "0.2"

int main(int argc, char **argv)
{
    try {
    	Cgicc formData;
    	const CgiEnvironment& cgienv = formData.getEnvironment();

    	string scommand, sparameter;
    	scommand = "";
    	sparameter = "";

    	string postdata =  cgienv.getPostData();
    	string encodedpostdata = cgicc::form_urldecode(postdata);

    	syslog( LOG_ERR, "EncodedPostData: %s",encodedpostdata.c_str());

       /* Commando find*/
       form_iterator command = formData.getElement("COMMAND");
       if((command != formData.getElements().end()))
       {
         scommand = (**command).c_str();
         // Checken to start the Daemon, thats a specail command
         if (scommand=="STARTDEAMON")
         {
        	 syslog( LOG_ERR, "Trying to start Deamon per web request...");
        	 execv(DEAMON_PATH,NULL);
         }
         // THE CGI VERSION INFO IS CALLED
         if (scommand=="MARELAB_CGI_VERSION")
         {
        	 cout << "content-type: text/html" << endl << endl;
        	 cout << "MARELAB_CGI_VERSION=" << marelab_cgi_version << endl;
        	 return 0;
          }
       }
       else
       {
    	  cout << "content-type: text/html" << endl << endl;
    	  cout << "Called without parameters CGI versio:" << marelab_cgi_version << endl;
          syslog( LOG_ERR, "Call without command...");
       }



       /*Getting all form Parameters of the request and
        * transforming it into a JSON string that
        * the deamon can understand...
        */
       vector <FormEntry> vf = formData.getElements();
       string parameterString;
       int trigger_comma = 0;
       for (unsigned int c=0; c < vf.size();c++ ){
			syslog(LOG_ERR, "POST ...[%s]-[%s]", vf[c].getName().c_str(),vf[c].getValue().c_str());
			if (vf[c].getName() == "PARAMETER") {
				if (trigger_comma == 0){
					parameterString = vf[c].getValue();
					trigger_comma = 1;
				}
				else{
					parameterString = parameterString +","+ vf[c].getValue();
				}
			}
			if ((vf[c].getName() != "PARAMETER") && (vf[c].getName() != "COMMAND")) {
				if (trigger_comma == 0) {
					parameterString = "\"" + vf[c].getName() + "\":\""+ vf[c].getValue() + "\"";
					trigger_comma = 1;
				}
				else{
					parameterString = parameterString + ",\"" + vf[c].getName() + "\":\""+ vf[c].getValue() + "\"";
				}
			}
       }

       cout << "CGI: " << parameterString << endl;
       /*
        * SENDING COMMAND & P	ARAMETER TO DEAMON AND WAIT
        * FOR RESPONSE OR TIMEOUT
        */
       string string2send;
       if (parameterString.compare(0,1,"[")==0)
    	   string2send = "{\"COMMAND\":\""+scommand+"\",\"PARAMETER\":"+parameterString+"}\0";
       else
    	   string2send = "{\"COMMAND\":\""+scommand+"\",\"PARAMETER\":{"+parameterString+"}}\0";
       cout << "CGI2DEAMON: " << string2send << endl;

       // Socket Communication
       ipccom sock;
       try
       {
    	   sock.openClient();
    	   syslog( LOG_ERR, "Client socket created successfull...");
    	   string msg;
    	   if (!sock.sendSock(string2send))
    		   syslog( LOG_ERR, "Client socket send failed...");
    	   else
    	   {
    		  syslog( LOG_ERR, "Client socket send [%s] ok...",string2send.c_str());
    		  if (sock.recvSockFromNucleus())
    		  {
    		  	syslog( LOG_ERR, "Recv return from server: OK");
    		  	cout << "content-type: text/html" << endl << endl;
    		  	cout << sock.getMsg().c_str();
    		  }
    	   }
    	   sock.closeServer();

       }
       catch(const char *Exception){
    	   	   	cout << "content-type: text/html" << endl << endl;
    	   	   	string errormsg = Exception;
    	   	   	cout << "{\"ERROR\":\"the server seems to be down reason: ["+errormsg+"] check the logs for more info\"}";
    	   		syslog( LOG_ERR, "the server seems to be down reason:  [%s] ... terminating",Exception );
    	   		sock.closeServer();
       }

       return 0;


    }
    catch(exception& e) {
       // handle any errors here.
       cout << "ERROR!!" << endl;
    }
    return 0;   // To avoid Apache errors.
}

