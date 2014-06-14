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
 * Name        : marelab-solar-deamon.cpp
 * Author      : Marc Philipp Hammermann
 * Version     :
 * Copyright © 2012 marc philipp hammermann  <marchammermann@googlemail.com>
 *
 * Description : This is the Deamon of the marelab system. Serving GUI and loading plugins
 *
 *
 *
 */

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

#include <sys/sysinfo.h>

#include "mconfig.h"





//#include "marelab/LedString.h"
//#include "marelab/LedTimerListe.h"
#include "marelab/IncomingMsg.h"
#include "json/json.h"
#include "json/writer.h"
#include "json/reader.h"
#include "marelab/ConfigNucleus.h"
#include "marelab/ipccom.h"
#include "marelab/PluginRegistry.h"
#include "marelab/MoonPhase.h"

// MODBUS INCLUDES
/*
#include "modbus/modbus.h"
#include "modbus/marelab_modbus.h"
#include "modbus/MBD.h"
#include "modbus/marelab_bus.h"
*/
#include "marelab/ConfigRegister.h"
#include "json/json.h"
#include "globals/IJsonSerializable.h"
#include "globals/LConnection.h"
#include "marelab/CJsonSerializer.h"
#include "marelab/mlog.h"
#include "marelab/DataLogger.h"

//LUA INCLUDING FOR SCRIPTING
#include <lua.hpp>

// On Linux, you must compile with the -D_REENTRANT option.  This tells
// the C/C++ libraries that the functions must be thread-safe
#ifndef _REENTRANT
#error ACK! You need to compile with _REENTRANT defined since this uses threads
#endif


using namespace std;

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

/*
 * Global marleab config objects
 */
ConfigNucleus nucleusConfig;

/*
 * Registry for all marelab plugins scanned during
 * startup of the deamon
 */
PluginRegistry *pluginRegistry;

/*
 * OBJ that are saved to a Config file
 * the ConfigRegister class does the job
 */
//****** Bus *bus = new Bus();				// marelab bus system
//LedTimerListe ledListe;				// List of all LED Strings




ConfigRegister configRegistry;


IncomingMsg   	msgin;				//
time_t 			tim;				// SystemTime

volatile int bStop = false;			// To stop threads

ipccom *ipcs;						// Linux IPC Sockets/Queue this is the communication channel cgi deamon cgi


// Global System Information
time_t marelab_gStartTime;   // Starttime of marelab

// Global LOGGER Service for Data aquisition
DataLogger *datalogger;


// Global Exit flag if true Deamon is quit
bool deamonexit = false;

/*
static void child_handler(int signum)
{
    switch(signum) {
    case SIGALRM: exit(EXIT_FAILURE); break;
    case SIGUSR1: exit(EXIT_SUCCESS); break;
    case SIGCHLD: exit(EXIT_FAILURE); break;
    }
}


static void daemonize( const char *lockfile )
{
    pid_t pid, sid, parent;
    int lfp = -1;

    // already a daemon
    if ( getppid() == 1 ) return;

    // Create the lock file as the current user
    if ( lockfile && lockfile[0] ) {
        lfp = open(lockfile,O_RDWR|O_CREAT,0640);
        if ( lfp < 0 ) {
        	string lockf = lockfile;
        	string errortxt =strerror(errno);
        	MLOG::log( "daemonize: unable to create lock file "+lockf+", code=" +i2str(errno)+"("+errortxt+")",__LINE__,__FILE__);
            exit(EXIT_FAILURE);
        }
    }

    // Drop user if there is one, and we were run as root
    if ( getuid() == 0 || geteuid() == 0 ) {
        struct passwd *pw = getpwnam( nucleusConfig.getCfRunAsUser().c_str());
        if ( pw ) {
        	string runsuer = nucleusConfig.getCfRunAsUser();
        	MLOG::log("daemonize: setting user to " + runsuer ,__LINE__,__FILE__);
            setuid( pw->pw_uid );
        }
    }

    // Trap signals that we expect to recieve
    signal(SIGCHLD,child_handler);
    signal(SIGUSR1,child_handler);
    signal(SIGALRM,child_handler);

    // Fork off the parent process
    pid = fork();
    if (pid < 0) {
    	MLOG::log("daemonize: unable to fork daemon, code=" +i2str(errno)+"("+strerror(errno)+")",__LINE__,__FILE__);
        exit(EXIT_FAILURE);
    }
    // If we got a good PID, then we can exit the parent process.
    if (pid > 0) {

        // Wait for confirmation from the child via SIGTERM or SIGCHLD, or
        //   for two seconds to elapse (SIGALRM).  pause() should not return.
        alarm(2);
        pause();

        exit(EXIT_FAILURE);
    }

    // At this point we are executing as the child process
    parent = getppid();

    // Cancel certain signals
    signal(SIGCHLD,SIG_DFL);
    signal(SIGTSTP,SIG_IGN);
    signal(SIGTTOU,SIG_IGN);
    signal(SIGTTIN,SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    signal(SIGTERM,SIG_DFL);

    // Change the file mode mask
    umask(0);

    // Create a new SID for the child process
    sid = setsid();
    if (sid < 0) {
    	MLOG::log("daemonize: unable to create a new session, code=" +i2str(errno)+"("+strerror(errno)+")",__LINE__,__FILE__);
        exit(EXIT_FAILURE);
    }

    // Change the current working directory.  This prevents the current
    //   directory from being locked; hence not being able to remove it.
    if ((chdir("/")) < 0) {
    	MLOG::log("daemonize: unable to change directory to '/', code=" +i2str(errno)+"("+strerror(errno)+")",__LINE__,__FILE__);
        exit(EXIT_FAILURE);
    }

    // Redirect standard files to /dev/null
    freopen( "/dev/null", "r", stdin);
    freopen( "/dev/null", "w", stdout);
    freopen( "/dev/null", "w", stderr);

    // Tell the parent process that we are A-okay
    kill( parent, SIGUSR1 );
}
*/
void SendMsgBack(string msg){
	if (!ipcs->send2cgi(msg))
		//	MLOG::log("Sending message back: "+msg,__LINE__,__FILE__);
	//else
			MLOG::log("ERROR sending back message :"+msg,__LINE__,__FILE__);
}



bool ParseIncomingMsg(string msg)
{
	//MLOG::log("ParseIncomingMsg: MSG GETS PARSE: \""+ msg+"\"",__LINE__,__FILE__);

	if (CJsonSerializer::Deserialize(&msgin,msg))
	{
		//MLOG::log("ParseIncomingMsg: MSG parsed successfull...",__LINE__,__FILE__);
		return true;
	}
	else
	{
		MLOG::log("ParseIncomingMsg: MSG parsed failed...",__LINE__,__FILE__);
	}
	return false;
}


/*
 * COMMANDS FROM THE SOCKET THREAD
 */
void mcu_BUSSAVE() {
	printf("PORTS & BUS SAVED\n");

	Json::Value para = msgin.getParameter();
	MLOG::log("mcu_BUSSAVE: " + para.toStyledString(),__LINE__,__FILE__);

	string port = para["PORT"].asString();
	//int baud = atoi(para["BAUD"].asString().c_str());
	//int databits = atoi(para["DATABITS"].asString().c_str());
	//int stopbits = atoi(para["STOPBITS"].asString().c_str());
	string parity = para["PARITY"].asString();

	//printf("WEB MSG BUSSAVE :%s,%d,%d,%s,%d",port.c_str(),baud,databits,parity[0],stopbits);
	//****** bus->setBaud(baud);
	//****** bus->setComport(port);
	//****** bus->setDatabits(databits);
	//****** bus->setParity(parity[0]);
	//****** bus->setStopbits(stopbits);

	configRegistry.writeConfig();

	string retmsg = "{\"COMMAND\":\"mcu_BUSSAVE\",\"PARAMETER\":[]}";
	SendMsgBack(retmsg);
}

/*
 * COMMANDS FROM THE SOCKET THREAD
 */
void mcu_BUSRESTORE() {
	string busstring;
	Json::Value busobj;
	//****** bus->Serialize( busobj);
	//****** busstring = busobj.toStyledString();
	MLOG::log("mcu_BUSRESTORE: Return "+busstring,__LINE__,__FILE__);

	string retmsg = "{\"COMMAND\":\"mcu_BUSSAVE\",\"PARAMETER\":"+busstring+"}";

	SendMsgBack(retmsg);
}

void mcu_PORTSCAN(){
	MLOG::log("mcu_PORTSCAN: PORT SCAN ACTIVATED",__LINE__,__FILE__);

	//****** list<string> l = bus->getComList();
	//****** list<string>::iterator it = l.begin();
	/*
	string ports;
	if (!l.empty())
		ports = "{\"port\":\""+*it + "\"}";
	while (it != l.end())
	{
		it++;
		if (it != l.end())
		{
			ports = ports + ",{\"port\":\""+*it + "\"}";
			cout << *it << endl;
		}
	}
	*/
	//******  string retmsg = "{\"COMMAND\":\"mcu_PORTSCAN\",\"PARAMETER\":["+ports+"]}";
	string retmsg = "{\"COMMAND\":\"mcu_PORTSCAN\",\"PARAMETER\":[""]}";

	SendMsgBack(retmsg);
}

void mcu_VERSION(){
	string ver = nucleusConfig.getCfNucleusVersion();
	string retmsg = "{\"COMMAND\":\"GET_mcu_VERSION\",\"PARAMETER\":\""+ver+"\"}";
	SendMsgBack(retmsg);
}

void mcu_BUSSCAN(){
	string returnval="";
	try{
		MLOG::log("mcu_BUSSCAN: Command:  mcu_BUSSCAN() .. ",__LINE__,__FILE__);
		//****** bus->ScanBus();

		string muuid;
		string name;
		// Generate the List with all BUS members and send it back to the GUI

		//****** int size = bus->getMBdList().size();
		MLOG::log("mcu_BUSSCAN: --Dump MBD LIST------------------------------------",__LINE__,__FILE__);
		/*for(int i=0; i<size; i++)
		{
			returnval = returnval + "[{\"ADRESS\":\""+ i2str(bus->getMBdList()[i].MBD_getAdress())  + "\"}";
			returnval = returnval + ",{\"MAJOR\":\""+ i2str(bus->getMBdList()[i].MBD_getMajorVersion())  + "\"}";
			returnval = returnval + ",{\"MINJOR\":\""+ i2str(bus->getMBdList()[i].MBD_getMinorVersion())  + "\"}";
			returnval = returnval + ",{\"NAME\":\""+ bus->getMBdList()[i].MBD_getName()  + "\"}";
			if (i== (size-1))
				returnval = returnval + ",{\"UUID\":\""+ bus->getMBdList()[i].MBD_getUUID()  + "\"}]";
			else
				returnval = returnval + ",{\"UUID\":\""+ bus->getMBdList()[i].MBD_getUUID()  + "\"}],";
		}
*/
		string retmsg = "{\"COMMAND\":\"mcu_PORTSCAN\",\"PARAMETER\":["+returnval+"]}";

		SendMsgBack(retmsg);
	}
	catch(string &Exception){
		MLOG::log("mcu_BUSSCAN: "+Exception+"... terminating",__LINE__,__FILE__);
		string retmsg = "{\"COMMAND\":\"mcu_PORTSCAN\",\"PARAMETER\":["+Exception+"]}";
		SendMsgBack(retmsg);
	}

}
/*
 * Scans the plugins and returns a JSON Object of founded Plugins
 */
void  mcu_PLUGINSCAN(){
	string returnval="";
	try{
		MLOG::log("mcu_BUSSCAN: Command:  mcu_BUSSCAN() .. ",__LINE__,__FILE__);
		//****** bus->ScanBus();

		string muuid;
		string name;
		// Generate the List with all BUS members and send it back to the GUI

		//****** int size = bus->getMBdList().size();
		/*
		MLOG::log("mcu_BUSSCAN: --Dump MBD LIST------------------------------------",__LINE__,__FILE__);
		for(int i=0; i<size; i++)
		{
			returnval = returnval + "[{\"PLUGINNAME\":\""+ i2str(bus->getMBdList()[i].MBD_getAdress())  + "\"}";
			returnval = returnval + ",{\"PLUGINVERSION\":\""+ i2str(bus->getMBdList()[i].MBD_getMajorVersion())  + "\"}";
			if (i== (size-1))
				returnval = returnval + ",{\"UUID\":\""+ bus->getMBdList()[i].MBD_getUUID()  + "\"}]";
			else
				returnval = returnval + ",{\"UUID\":\""+ bus->getMBdList()[i].MBD_getUUID()  + "\"}],";
		}
		*/
		string retmsg = "{\"COMMAND\":\"mcu_BUSSCAN\",\"PARAMETER\":["+returnval+"]}";

		SendMsgBack(retmsg);
	}
	catch(string &Exception){
		MLOG::log("mcu_BUSSCAN: "+Exception+"... terminating",__LINE__,__FILE__);
		string retmsg = "{\"COMMAND\":\"mcu_PORTSCAN\",\"PARAMETER\":["+Exception+"]}";
		SendMsgBack(retmsg);
	}

}




/*
 * Saves the nuclueus plugin logic -> adapter binding
 */
void mcu_NUCLEUS_CONFIGSAVE(){

	    Json::Value para = msgin.getParameter();
	    Json::Value element;
	    MLOG::log("mcu_NUCLEUS_CONFIGSAVE: " + para.toStyledString(),__LINE__,__FILE__);
	    PluginObject* logicplugin;
	    PluginObject* adapterplugin;
	    unsigned int i;
		for (i = 0; i <  para.size(); i++ )
		{
		  element = para[i];
		  logicplugin = pluginRegistry->GetPluginWithName(element["Name"].asString());
		  adapterplugin = pluginRegistry->GetAdapterWithName(element["Adapter"].asString());
		  logicplugin->adapter = adapterplugin->plugin;
		}

		configRegistry.writeConfig();
	    string retmsg = "{\"COMMAND\":\"mcu_NUCLEUS_CONFIGSAVE\"}";
		SendMsgBack(retmsg);
}

/*
 * Itterates throw the plugin registry and gives back all pluginnames
 */
void PLUGIN_JAVASCRIPTFILES(string command){

	string namelist = pluginRegistry->JSONgetPluginFileNames();
	MLOG::log("PLUGIN_JAVASCRIPTFILES: sending these Plugin Names: "+namelist ,__LINE__,__FILE__);
	string retmsg = "{\"COMMAND\":\"PLUGIN_JAVASCRIPTFILES\",\"PARAMETER\":"+namelist+"}";
	SendMsgBack(retmsg);
}


/*
 * Reading config of a plugin
 */
void READ_CONFIG(string command, string plugin_name) {
	string output;

	PluginObject* pluginobj = pluginRegistry->GetPluginWithName(plugin_name);
	if (pluginobj!=NULL){
		pluginobj->plugin->GetConfigAsJSON(output);
		SendMsgBack(output);
	}
	else{
		SendMsgBack("UNKNOWN PLUGIN :" +plugin_name);
	}
}

/*
 * Saving config of a plugin
 */
void SAVE_CONFIG(string command, string plugin_name,Json::Value& para) {
	string output;
	PluginObject* pluginobj = pluginRegistry->GetPluginWithName(plugin_name);
	pluginobj->plugin->SetConfigAsJSON(para);
	configRegistry.writeConfig();
	SendMsgBack("{COMMAND=SAVE_CONFIG}");
}

/*
 * Rrturns the HTML for a adapter to config it s parameters
 */
void ADAPTER_GET_CONFIG_HTML(string command, string plugin_name,Json::Value& para) {
	string output;
	PluginObject* pluginobj = pluginRegistry->GetPluginWithName(plugin_name);
	// Finding the configured Adapter
	output = pluginobj->adapter->getConfigHTML(plugin_name);
	// Getting the HTML page code for the Adapter Config

	// Returning the HTML page code to the client for showing
	SendMsgBack("{\"COMMAND\":\"ADAPTER_GET_CONFIG_HTML\",\"HTML\":\""+output+"\"}");
	//SendMsgBack("{ADAPTER_HTML:\""+plugin_name+"\"}");
}

// Get Infos of the installed plugins
void GET_PLUGININFO(string command, string plugin_name){
	string output;
	Json::Value plugins;
	pluginRegistry->SerializeAjax(plugins);
	string plugs = plugins.toStyledString();
	SendMsgBack(plugs);
}

// Get Infos of the installed Adapter plugins
void GET_ADAPTER_PLUGININFO(string command, string plugin_name){
	string output;
	Json::Value plugins;
	pluginRegistry->SerializeAdapter(plugins);
	string plugs = plugins.toStyledString();
	SendMsgBack(plugs);
}



std::string s2str(string i) {
 std::ostringstream buffer;
 buffer << i;
 return buffer.str();
}


/*
 * sending Log messages back to the client that the deamon has collected up to now
 * remeber that the Client-> Server communication is initiated by the client
 */
void deamon_MLOG()
{
	try
	{
		string logmsg = encodeForJSON(MLOG::getLogMsg());
		logmsg = "\"" + logmsg +"\"";
		string retmsg = "{\"COMMAND\":\"deamon_LOG\",\"PARAMETER\":"+logmsg+"}";
		MLOG::log("deamon_MLOG() ret msg= "+retmsg ,__LINE__,__FILE__);
		SendMsgBack(retmsg);

	}
	catch(string &Exception){
		MLOG::log("deamon_LOG: ERROR while transmit log : "+Exception+ "...",__LINE__,__FILE__);
	}

}


/*
 * gets the list of connectors of the plugin -> pluginName
 * and returns it as JSON Object
 */
Json::Value GetPluginConnectors(string pluginName){
	Json::Value pluginConnectorList = Json::nullValue;
	if (pluginRegistry->GetPluginWithName(pluginName)!=NULL)
	{
		if (pluginRegistry->GetPluginWithName(pluginName)->GetTypeOfPlugin()=="LOGIC"){
			pluginConnectorList = pluginRegistry->GetPluginWithName(pluginName)->plugin->ConGetConnectorList();
			return pluginConnectorList;
		}
	}
	if (pluginRegistry->GetAdapterWithName(pluginName)!=NULL){
		if (pluginRegistry->GetAdapterWithName(pluginName)->GetTypeOfPlugin()=="ADAPTER"){
			pluginConnectorList = pluginRegistry->GetAdapterWithName(pluginName)->plugin->ConGetConnectorList();
			return pluginConnectorList;
		}
	}
	return pluginConnectorList;
}

// Returns a JSON Obj containing all phy Adapter Connetors
Json::Value  GetPhysicalConnectors(){
	Json::Value pluginConnectorList = Json::nullValue;
	Json::Value retJson= Json::nullValue;;
	int countOfAdapters = pluginRegistry->getAdapterCount();
	cout << pluginRegistry->DebugAdapterList() << endl;
	for (int i=0; i < countOfAdapters; i++){
		string name = pluginRegistry->GetAdapterEntry(i)->plugin->getName();
		if (pluginRegistry->GetAdapterEntry(i)->plugin->getTypeOfPlugin()=="ADAPTER"){
			pluginConnectorList = pluginRegistry->GetAdapterEntry(i)->plugin->ConGetConnectorList();
			retJson["PHYSICAL_CONNETOR_LIST"].append(pluginConnectorList);
			cout << "Adapter:"<< name << endl;
			cout << "Adapter:"<< pluginRegistry->GetAdapterWithName(name)->plugin->ConGetConnectorList().toStyledString() << endl;
		}
	}
	cout << "GetPhysicalConnectors:"<< pluginConnectorList.toStyledString() << endl;

	return retJson;
}

// Returns a JSON Obj containing all logical Adapter Connetors like dimme, stream etc.
// These are the Function plugins that have connectors that needed to be tight to a
// physical connector.
// -> Logic Adapter/Plugin implements the functionality
// -> Physical Adapter/Plugin implements the hardware ports that can be used by logic
Json::Value  GetLogicalConnectors(){
	Json::Value pluginConnectorList = Json::nullValue;
	Json::Value retJson= Json::nullValue;;
	int countOfLogicPluins = pluginRegistry->getPluginCount();
	cout << pluginRegistry->DebugLogicList() << endl;
	for (int i=0; i < countOfLogicPluins; i++){
		string name = pluginRegistry->GetPluginEntry(i)->plugin->getName();
		if (pluginRegistry->GetPluginEntry(i)->plugin->getTypeOfPlugin()=="LOGIC"){
			pluginConnectorList = pluginRegistry->GetPluginEntry(i)->plugin->ConGetConnectorList();
			retJson["LOGICAL_CONNETOR_LIST"].append(pluginConnectorList);
			cout << "Adapter:"<< name << endl;
			cout << "Adapter:"<< pluginRegistry->GetPluginWithName(name)->plugin->ConGetConnectorList().toStyledString() << endl;
		}
	}
	cout << "GetPhysicalConnectors:"<< pluginConnectorList.toStyledString() << endl;

	return retJson;
}

/*
 * MAIN LOOP FOR COMUNICATION WITH THE MARELAB SYSTEM
 * THE DAEMON HAS A SOKET LISTENER for ipc messages. These ipc Messages controls
 * the action that is send over the marelab bus (USB/RS485) to the mBD Devices.
 * A return is send after the operation back to the sender.
 *
 * Currently a CGI Program converts WEB command task, activated over the web GUI
 * to the deamon. But as long as the sender uses the defined format. Any application
 * able to send ipc messages can control and talk to the marelab system.
 *
 * ALL COMMANDS ARE SEND AS JSON MESSAGE FORMAT
 * COMMUNICATION HANDLING /////////////////////////////////////////////////////////////////
 * ///////////////////////////////////////////////////////////////////////////////////////////
 * //POST: COMMAND='COMMANDNAME' & PARAMETER = '[JSON_ENCODED_PARAMETERARRAY,..]'
 * //JSON_ENCODED_PARAMETERARRAY: [NAME:VALUE,NAMEn:VALUEn]
 */
void *marelab_Socket_thread(void *)
{

	while (!deamonexit) /*Endlosschleife*/
	{
	// Incoming Msg per socket
	if (ipcs->recvSock()) {

		//MLOG::log("marelab_Socket_thread: MSG ["+ipcs->getMsg()+"]...",__LINE__,__FILE__);
		if (ParseIncomingMsg(ipcs->getMsg()) == true) {
			string command = msgin.getCommand().asString();

			if (command.compare("EXIT") == 0) {
				MLOG::log("marelab_Socket_thread: marelab deamon exit command ...",__LINE__,__FILE__);
				deamonexit = true;
			}

			else if (command.compare("mcu_VERSION") == 0) {
				string ver = nucleusConfig.getCfNucleusVersion();
				MLOG::log("marelab_Socket_thread: mcu_Version:"+ ver +" ...",__LINE__,__FILE__);
				mcu_VERSION();

			}

			/* Scans the bus for mBD Devices and returns them to the GUI */
			else if (command.compare("mcu_BUSSCAN") == 0) {
				MLOG::log("marelab_Socket_thread: mcu_BUSSCAN() ...",__LINE__,__FILE__);
				mcu_BUSSCAN();

			}

			/* Scans the bus for mBD Devices and returns them to the GUI */
			else if (command.compare("mcu_PLUGINSCAN") == 0) {
				MLOG::log("marelab_Socket_thread: mcu_BUSSCAN() ...",__LINE__,__FILE__);
				mcu_PLUGINSCAN();

			}

			/* Get all Ports on the mCU */
			else if (command.compare("mcu_PORTSCAN") == 0) {
				MLOG::log("marelab_Socket_thread: mcu_PORTSCAN() ...",__LINE__,__FILE__);
				mcu_PORTSCAN();

			}

			/* TEST FOR THE NEW CONFIG PERSISTENT LAYER */
			else if (command.compare("mcu_BUSSAVE") == 0) {
				MLOG::log("marelab_Socket_thread: mcu_BUSSAVE() ...",__LINE__,__FILE__);
				mcu_BUSSAVE();

			}
			else if (command.compare("mcu_BUSRESTORE") == 0) {
				MLOG::log("marelab_Socket_thread: mcu_BUSRESTORE() ...",__LINE__,__FILE__);
				mcu_BUSRESTORE();
			}
			else if (command.compare("mcu_NUCLEUS_CONFIGSAVE") == 0) {
				MLOG::log("marelab_Socket_thread: mcu_NUCLEUS_CONFIGSAVE() ...",__LINE__,__FILE__);
				mcu_NUCLEUS_CONFIGSAVE();
			}
			/* Send Log Messages to the client */
			else if (command.compare("deamon_LOG") == 0) {
				// Remove log Info b
				//MLOG::log("marelab_Socket_thread: deamon_MLOG() ...",__LINE__,__FILE__);
				deamon_MLOG();
			}

			/* Send TIME to the client */
			else if (command.compare("GETTIME") == 0) {
				string datum;
				long uptime;
				time_t t = time(0);   // get time now
				struct tm *now = localtime( & t );

				datum = i2str((now->tm_year + 1900)) + '-'
						        + i2str((now->tm_mon + 1)) + '-'
						        + i2str(now->tm_mday)+ ' '
						        + i2str(now->tm_hour)+ ':'
						        + i2str(now->tm_min) + ':'
						        + i2str(now->tm_sec);


			   uptime = t - marelab_gStartTime;

			   //MLOG::log("GETTIME ..." + datum + " UpTime" + i2str(uptime),__LINE__,__FILE__);

			   SendMsgBack("{\"COMMAND\":\"GETTIME\","
					       "\"YEAR\":\""+i2str((now->tm_year + 1900))+"\","
					       "\"MONTH\":\""+i2str((now->tm_mon+1))+"\","
					       "\"DAY\":\""+i2str((now->tm_mday))+"\","
					       "\"HOUR\":\""+i2str((now->tm_hour))+"\","
					       "\"MIN\":\""+i2str((now->tm_min))+"\","
					       "\"SEC\":\""+i2str((now->tm_sec))+"\","
			   	   	   	   "\"UPTIME\":\""+i2str((uptime))+"\"}");
			}

			/* Send SYSINFO to the client */
			else if (command.compare("SYSINFO") == 0) {

				  struct sysinfo linuxinfo;
				  sysinfo (&linuxinfo);


				 //  MLOG::log("GETTIME ..." + datum + " UpTime" + i2str(uptime),__LINE__,__FILE__);

				   SendMsgBack("{\"COMMAND\":\"SYSINFO\","
								       "\"UPTIME\":\""+		i2str(linuxinfo.uptime)+"\","
								       "\"LOAD1\":\""+		i2str(linuxinfo.loads[0] )+"\","
								       "\"LOAD5\":\""+		i2str(linuxinfo.loads[1])+"\","
								       "\"LOAD15\":\""+		i2str(linuxinfo.loads[2])+"\","
								       "\"FREERAM\":\""+	i2str(linuxinfo.freeram)+"\","
								       "\"SHAREDRAM\":\""+	i2str(linuxinfo.sharedram)+"\","
								       "\"BUFFERRAM\":\""+	i2str(linuxinfo.bufferram)+"\","
								       "\"TOTALSWAP\":\""+	i2str(linuxinfo.totalswap)+"\","
								       "\"FREESWAP\":\""+	i2str(linuxinfo.freeswap)+"\","
								       "\"PROCS\":\""+		i2str(linuxinfo.procs)+"\","
								       "\"TOTALHIGH\":\""+	i2str(linuxinfo.totalhigh)+"\","
								       "\"FREEHIGH\":\""+	i2str(linuxinfo.freehigh)+"\"}");
			}


			// GET ALL SETUPDATA FROM THE CONFIG AS JSON STRING
			else if (command.compare("GETMARELABSETUPDATA") == 0) {
				string DAEMON_LOCK_PATH  = nucleusConfig.getCfDaemonLockPath();
				string DAEMON_NAME = nucleusConfig.getCfDaemonName();
				string DAEMON_NAME_LOCK= nucleusConfig.getCfDaemonNameLock();
				string DEAMON_PATH= nucleusConfig.getCfDeamonPath();
				string NUCLEUS_VERSION= nucleusConfig.getCfNucleusVersion();
				string PLUGINDIR= nucleusConfig.getCfPlugindir();
				string RUN_AS_USER= nucleusConfig.getCfRunAsUser();
				string SOCK_PATH= nucleusConfig.getCfSockPath();
				string LOG_LEVEL=nucleusConfig.getCfLogLevel();
				string DB_PATH=nucleusConfig.getCfDbPath();
				SendMsgBack("{\"COMMAND\":\"SENDMARELABSETUPDATA\","
						    "\"CONFIGDATA\":["
							"{\"DAEMON_LOCK_PATH\":\""+		DAEMON_LOCK_PATH +"\"},"
							"{\"DAEMON_NAME\":\""+			DAEMON_NAME+"\"},"
							"{\"DAEMON_NAME_LOCK\":\""+		DAEMON_NAME_LOCK+"\"},"
							"{\"DEAMON_PATH\":\""+			DEAMON_PATH+"\"},"
							"{\"NUCLEUS_VERSION\":\""+		NUCLEUS_VERSION+"\"},"
							"{\"RUN_AS_USER\":\""+			RUN_AS_USER+"\"},"
							"{\"SOCK_PATH\":\""+				SOCK_PATH+"\"},"
							"{\"LOG_LEVEL\":\""+				LOG_LEVEL+"\"},"
							"{\"DB_PATH\":\""+				DB_PATH+"\"},"
							"{\"PLUGINDIR\":\""+				PLUGINDIR+"\"}]}");
			}

			//SAVE MARELAB_CONFIG DATA BACK
			else if (command.compare("SETMARELABSETUPDATA") == 0) {
				Json::Value para = msgin.getParameter();
				Json::Value element;
				MLOG::log("NUCLEUS_CONFIGSAVE: " + para.toStyledString(),__LINE__,__FILE__);
				Json::Value  memberVAL =  para["MARELAB_CONFIG"];

				for (unsigned int j = 0; j <  memberVAL.size(); j++ )
				{
					Json::Value entry =  memberVAL[j];
					Json::Value::Members member = entry.getMemberNames();
					for(unsigned int i=0; i < member.size(); ++i)
					{
						string memberName = member[i];
						if (memberName == "DAEMON_NAME")
							nucleusConfig.setCfDaemonName(memberVAL[j][memberName].asString());
						else if (memberName == "DAEMON_LOCK_PATH")
							nucleusConfig.setCfDaemonLockPath(memberVAL[j][memberName].asString());
						else if (memberName == "DAEMON_NAME_LOCK")
							nucleusConfig.setCfDaemonNameLock(memberVAL[j][memberName].asString());
						else if (memberName == "DEAMON_PATH")
							nucleusConfig.setCfDeamonPath(memberVAL[j][memberName].asString());
						else if (memberName == "NUCLEUS_VERSION")
							nucleusConfig.setCfNucleusVersion(memberVAL[j][memberName].asString());
						else if (memberName == "RUN_AS_USER")
							nucleusConfig.setCfRunAsUser(memberVAL[j][memberName].asString());
						else if (memberName == "SOCK_PATH")
							nucleusConfig.setCfSockPath(memberVAL[j][memberName].asString());
						else if (memberName == "LOG_LEVEL")
							nucleusConfig.setCfLogLevel(memberVAL[j][memberName].asString());
						else if (memberName == "PLUGINDIR")
							nucleusConfig.setCfPlugindir(memberVAL[j][memberName].asString());
					}
				}

				configRegistry.writeConfig();
				SendMsgBack("{COMMAND=CONFIGSAVE_OK}");
			}


			/*
			 * Plugin Section these functions are imported into the daemon by
			 * dynamic loading of plugins.
			 */
			/* Gets a list of all JavaScript files needed by the installed Plugins */
			else if (command.compare("PLUGIN_JAVASCRIPTFILES") == 0) {
				MLOG::log("marelab_Socket_thread: GET THE PLUGIN_JAVASCRIPTFILES...",__LINE__,__FILE__);
				PLUGIN_JAVASCRIPTFILES(command);
			}
			/* Logic Plugin Info Name & Version */
			else if (command.compare("GET_PLUGININFO") == 0) {
				MLOG::log("marelab_Socket_thread: GET THE Logic PLUGIN INFO ...",__LINE__,__FILE__);
				GET_PLUGININFO(command,msgin.getPlugin());
			}

			/* Adapter Plugin Info Name & Version */
			else if (command.compare("GET_ADAPTERINFO") == 0) {
				MLOG::log("marelab_Socket_thread: GET THE Adapter PLUGIN INFO ...",__LINE__,__FILE__);
				GET_ADAPTER_PLUGININFO(command,msgin.getPlugin());
			}

			/* Read Plugin Config */
			else if (command.compare("READ_CONFIG") == 0) {
				MLOG::log("marelab_Socket_thread: PLUGIN wants to READ_CONFIG ...",__LINE__,__FILE__);
				READ_CONFIG(command, msgin.getPlugin());
			}
			/* Save Plugin settings of a single Plugin */
			else if (command.compare("SAVE_CONFIG") == 0) {
				//MLOG::log("marelab_Socket_thread: SAVELEDROW(command) ...",__LINE__,__FILE__);
				Json::Value para = msgin.getParameter();
				MLOG::log("marelab_Socket_thread: SAVE_CONFIG("+msgin.getPlugin()+") ...",__LINE__,__FILE__);
				SAVE_CONFIG(command,msgin.getPlugin(),para);
			}
			/* Save Plugin settings of a single Plugin */
			else if (command.compare("ADAPTER_GET_CONFIG_HTML") == 0) {
				Json::Value para = msgin.getParameter();
				MLOG::log("marelab_Socket_thread: ADAPTER_GET_CONFIG_HTML("+ msgin.getPlugin() + ") ...", __LINE__,__FILE__);
				ADAPTER_GET_CONFIG_HTML(command,msgin.getPlugin(),para);
				//SAVE_CONFIG(command, msgin.getPlugin(), para);
			}


			/*
			 * ******************************************************************************
			 * This part is responsible for Handling the logical & pyhsical Connectors
			 */
			else if (command.compare("GET_CONNECTOR_FOR_PLUGIN") == 0) {
				Json::Value para = msgin.getParameter();
				//MLOG::log("marelab_Socket_thread: GET_CONNECTOR_FOR_PLUGIN("+ msgin.getPlugin() + ") ...", __LINE__,__FILE__);
				string connectorList = GetPluginConnectors(para["PLUGINNAME"].asString()).toStyledString();
				SendMsgBack("{\"COMMAND\":\"GET_CONNECTOR_FOR_PLUGIN\",\"CONNECTOR\":"+connectorList+"}");
			}
			else if (command.compare("GET_ALL_PHYSICAL_CONNECTORS") == 0) {
				Json::Value para = msgin.getParameter();
				string connectorList = GetPhysicalConnectors().toStyledString();
				SendMsgBack("{\"COMMAND\":\"GET_ALL_PHYSICAL_CONNECTORS\",\"CONNECTOR\":"+connectorList+"}");
			}
			else if (command.compare("GET_ALL_LOGICAL_CONNECTORS") == 0) {
				Json::Value para = msgin.getParameter();
				string connectorList = GetLogicalConnectors().toStyledString();
				SendMsgBack("{\"COMMAND\":\"GET_ALL_LOGICAL_CONNECTORS\",\"CONNECTOR\":"+connectorList+"}");
			}

			else {
				/*
				 * Non of the Base Commands got triggerd so we go throu every Logic Plugin
				 * and try to trigger the function there
				 */

				 Json::Value para = msgin.getParameter();

				 MLOG::log("PARAMETER FOR PLUGINS :"+para.toStyledString(),__LINE__,__FILE__);

				 for (int LogicPluginNo = 0;LogicPluginNo < pluginRegistry->getPluginCount();LogicPluginNo++) {
					MLOG::log("FOR PLUGIN :"+pluginRegistry->GetPluginEntry(LogicPluginNo)->plugin->getName(),__LINE__,__FILE__);
					pluginRegistry->getPluginObject(LogicPluginNo)->Command(pluginRegistry->GetPluginEntry(LogicPluginNo)->adapter,para);
				 }

				 SendMsgBack("{\"COMMAND\":\"PLUGIN\",\"SCAN\":\"OK\"}");




			}/* else {
			 syslog(MLOG_ERR, "COMMAND [%s] unknown  ...",command.c_str());
			 // Sending Back Result for the incoming msg
			 if (ipcs.sendSockServer("RETURN FORM DAEMON"))
			 syslog( MLOG_ERR, "Send msg to client..");
			 }*/
		}
		// MSG CAN'T BE TRANSLATED
		else{
			MLOG::log("marelab_Socket_thread: MSG ["+ipcs->getMsg()+"] unknowen  ...",__LINE__,__FILE__);
			string retmsg = "Received MSG is unknown by the deamon";
			SendMsgBack(retmsg);
		}
	}
	// Incomig MSG can't be parsed
	else{
		MLOG::log("marelab_Socket_thread: MSG ["+ipcs->getMsg()+"] unknowen  ...",__LINE__,__FILE__);
		// Sending Back Result for the incoming msg
		string retmsg = "Received MSG is unknown by the deamon";
		SendMsgBack(retmsg);
	}
	}
	ipcs->closeServer();
	pthread_exit(0);
}


void marelab_daemon_entry_point()
{
	MLOG::log( "marelab_daemon_entry_poin: marelab deepblue deamon starting now...",__LINE__,__FILE__ );
	try{
		ipcs = new ipccom(&nucleusConfig);
		ipcs->openServer();
		MLOG::log( "NUCLEUS SOCKET SERVER CREATED ...",__LINE__,__FILE__ );
	}
	catch(string &Exception){
		MLOG::log( "marelab_daemon_entry_poin: ERROR="+Exception+ " ... terminating deamon...",__LINE__,__FILE__ );
		ipcs->closeServer();
		delete ipcs;
	}


	/* COMMAND RECV THREAD */
	pthread_t mythread;
	//int code =
	pthread_create(&mythread,NULL, marelab_Socket_thread,NULL);

	/* ENDLESS LOOP FOR PROCESSING */
	//int tempp=0;
	MoonPhase moon;


	/////////////////////////////////////////////////////////////////////////
	// This is the Daemon Loop                                             //
	/////////////////////////////////////////////////////////////////////////
	while (!deamonexit)
	{


		time(&tim);
		//tm *timeNow = localtime(&tim);						// Get the time now
		//syslog(MLOG_ERR, "COMMAND [%s] unknown  ...",command.c_str());
		/* DO MARELAB ACTIONS CONTROLLING HARDWARE DEVICES CONECTED TO MARELAB          */
		/* Iterate over the LedChannels and change the dimm values if needed            */

		/******************************************************************
		 * CALLS ALL INSTALLED PLUGINS -> work FUNCTION
		 */

		Json::Value test;
		for (int LogicPluginNo=0; LogicPluginNo < pluginRegistry->getPluginCount() ; LogicPluginNo++){
			// Gets a logic Plugin & performance the work
			pluginRegistry->getPluginObject(LogicPluginNo)->work(localtime(&tim),pluginRegistry->GetPluginEntry(LogicPluginNo)->adapter,test);
		}
		sleep(2);	// lets sleep a few ticks

	}
	ipcs->closeServer();
	delete ipcs;
}



int main( int argc, char *argv[] ) {
	string LUA_INIT_SCRIPT = "../marelab-scripts/init.lua";
	string MARELAB_CONFIG_FILE = "";
	// START LUA
	// create new Lua state

	// check if config file is used to start
	if (0 == strcmp(argv[1], "-config")) {
		// cout << "Filename:" << argv[2];
		// getting destination to my config from cmd line
		// We assume argv[1] is a filename to open
		FILE *file = fopen(argv[2], "r");

		/* fopen returns 0, the NULL pointer, on failure */
		if (file == 0) {
			printf("Could not open the marelab.conf file exiting ...\n");
			exit(-1);
		} else {
			fclose(file);
		}
		MARELAB_CONFIG_FILE = argv[2];
		cout << "marelab config file found by paramter : " << MARELAB_CONFIG_FILE << endl;
		configRegistry.setConfigFile(MARELAB_CONFIG_FILE);
	}


	lua_State *lua_state;
	lua_state = luaL_newstate();

	// load Lua libraries
	static const luaL_Reg lualibs[] =
			{ { "base", luaopen_base }, { NULL, NULL } };

	const luaL_Reg *lib = lualibs;
	for (; lib->func != NULL; lib++) {
		lib->func(lua_state);
		lua_settop(lua_state, 0);
	}


	// First action add the global Config Object
	configRegistry.addObj(&nucleusConfig);
	// Read only a part of the ConfigFile to have a
	// a running nucleus some parts can only be used
	// after plugins are loaded.
	configRegistry.readConfigPart(&nucleusConfig);
	cout << "READ CONFIG OK" << endl;


	// Initilize global DataLogger
	datalogger = new DataLogger(nucleusConfig.getCfDbPath());
	//27sec for 9000 entrys
		//for (int i=0;i< 9000;i++)
		//{
		//	time_t t1=time(NULL);
		//	datalogger->AddLogData("PH1",t1,0.1);
		//}

		//datalogger->Export("TEST",time(NULL));

    /* Initialize the logging interface */
	openlog( nucleusConfig.getCfDaemonName().c_str(), LOG_PID, LOG_LOCAL5 );

    MLOG::log("Starting marelab-deepblue-deamon...",__LINE__,__FILE__);


    bool daemon = true;
    if(0 == strcmp(argv[argc-1],"-?"))
    {
    	cout << endl;
    	cout << "Help     -?" << endl;
    	cout << "NoDaemon -ND" << endl;
    	cout << endl;
    	daemon = false;
    	closelog();
    	return 0;
    }
    else if(0 == strcmp(argv[argc-1],"-ND"))
    {
    	MLOG::log("Starting without getting Daemon",__LINE__,__FILE__ );
    	daemon = false;
    }
    if(0 == strcmp(argv[argc-1],"-DEV"))
    {
		// starting in developer mode under eclipse
    	// its needed because the ref to the directorys
    	// are different
    	LUA_INIT_SCRIPT = "marelab-scripts/init.lua";
    }
    if(0 == strcmp(argv[argc-1],"-config"))
    {
    	// getting destination to my config from cmd line
    	// We assume argv[1] is a filename to open
    	FILE *file = fopen( argv[2], "r" );

		/* fopen returns 0, the NULL pointer, on failure */
		if (file == 0) {
			printf("Could not open the marelab.conf file exiting ...\n");
			exit(-1);
		} else {
			fclose(file);
		}
		MARELAB_CONFIG_FILE = argv[2];
		cout << "marelab config file found : " << MARELAB_CONFIG_FILE << endl;
    }


    //configRegistry.addObj(&marlabConfig);
    /*
     * The Registry for marelab plugins
     */
    pluginRegistry = new PluginRegistry(&nucleusConfig);
    // Scans the plugins dir and adds founded plugs to the plugin registry
    pluginRegistry->ScanForPlugins();

    // Adding Plugin Registry for Saving Logic->Adapter bindings to the config registry
    configRegistry.addObj(pluginRegistry);

    /*
     * Adding all objects that needs config values from the Config File
     */
    //******  configRegistry.addObj(bus);
    //pluginRegistry->PluginsAddToConfig(&configRegistry);
    //configRegistry.addObj(&ledListe);

    configRegistry.readConfig();



    //******  if (bus->ConnectBus())
    //****** 	bus->ScanBus();


    /*
     *
     * Getting all device plugins
     *



    cout << "Loading symbol sensor devices...\n";

    void* tempsensor = dlopen("/mnt/arm-marelab-dev/marelab-deepblue/marelab-devices/Debug/libmarelab-devices.so", RTLD_LAZY);

    if (!tempsensor) {
       cout << "Cannot open library: " << dlerror() << '\n';
       return 1;
    }


    // load the symbols
  //create_t* create_triangle = (create_t*) dlsym(triangle, "create");
    create_t* create_sensor = (create_t*) dlsym(tempsensor, "create");
    const char* dlsym_error = dlerror();
    if (dlsym_error) {
    	 cout << "Cannot load symbol create: " << dlsym_error << '\n';
        return 1;
    }

    destroy_t* destroy_sensor = (destroy_t*) dlsym(tempsensor, "destroy");
    dlsym_error = dlerror();
    if (dlsym_error) {
    	 cout << "Cannot load symbol destroy: " << dlsym_error << '\n';
        return 1;
    }

    //polygon* poly = create_triangle();
    // create an instance of the class
    sensor* sens = create_sensor();

    // use the class
    string name = sens->getName();
    cout << "Name of loaded pugin: " << name << '\n';

    // destroy the class
    destroy_sensor(sens);

    // unload the triangle library
    dlclose(tempsensor);
     */


    marelab_gStartTime = time(NULL);   // sets the starttime of marelab deamon

    /* One may wish to process command line arguments here */



    /* Daemonize */
    if(daemon)
    	//daemonize( DAEMON_LOCK_PATH DAEMON_NAME_LOCK );

    // run the Lua int script
    luaL_dofile(lua_state, LUA_INIT_SCRIPT.c_str());

    /* Now we are a daemon -- do the work for which we were paid */
    marelab_daemon_entry_point();

    /* Finish up */
    MLOG::log("marelab deamon exit ...." ,__LINE__,__FILE__);

    // close global data logger

    delete pluginRegistry;
    delete datalogger;
    // close the Lua state
    lua_close(lua_state);
    closelog();
    return 0;
}
