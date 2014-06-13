#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <cstdio>
#include <errno.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <linux/serial.h>

#include <iostream>
#include <list>

#include "marelab_modbus.h"
#include "marelab_bus.h"

#include "../globals/json/json.h"
#include "../IJsonSerializable.h"

//#include "../mlog.h"

namespace std
{

	Bus::Bus  ()
	{
		baud = 9600;
		databits = 8;
		parity = 'N';
		stopbits = 1;
		ctx = NULL;
		connected = false;

	}


	Bus::~Bus()
	{

		//MLOG::log("Bus: UNCONNECT BUS ....",__LINE__,__FILE__);
		modbus_close(ctx);
		modbus_free(ctx);
	}

	bool Bus::isConnected(){
		return connected;
	}

	bool Bus::ConnectBus(char *port,int baudrate)
	{
		//MLOG::log("Bus: CONNECT TO BUS ...." ,__LINE__,__FILE__);
		comport = port;
		baud = baudrate;
		if (!isConnected()){
			ctx = modbus_new_rtu(comport.c_str(),baud,(char)parity.c_str()[0],databits,stopbits);
			if (modbus_connect(ctx) != 0)
			{
				modbus_close(ctx);
				modbus_free(ctx);
				string errstr =  modbus_strerror(errno);
				//MLOG::log("Error Bus:" + errstr,__LINE__,__FILE__);
				connected = false;
				return false;
			}
		}
		/* Define a new and too short timeout! */
		/*
		struct timeval response_timeout;
		modbus_get_response_timeout(ctx, &response_timeout);
		//printf( "Timeout sex %d usec %d ",response_timeout.tv_sec,response_timeout.tv_usec);
		response_timeout.tv_sec = 0;
		response_timeout.tv_usec = 50000;

		modbus_set_response_timeout(ctx, &response_timeout);
		*/
		connected = true;
		return true;
	}


	bool Bus::ConnectBus()
		{
			//MLOG::log("Bus: TRY TO CONNECT TO BUS ....",__LINE__,__FILE__);
			if (!isConnected()){
				ctx = modbus_new_rtu(comport.c_str(),baud,(char)parity.c_str()[0],databits,stopbits);
				if (modbus_connect(ctx) != 0)
				{
					modbus_close(ctx);
					modbus_free(ctx);
					string errstr =  modbus_strerror(errno);
					//MLOG::log("Error Bus:" + errstr,__LINE__,__FILE__);
					connected = false;
					return false;
				}
			}
			/* Define a new and too short timeout! */
			/*struct timeval response_timeout;
			modbus_get_response_timeout(ctx, &response_timeout);
			//printf( "Timeout sex %d usec %d ",response_timeout.tv_sec,response_timeout.tv_usec);
			response_timeout.tv_sec = 0;
			response_timeout.tv_usec = 50000;

			modbus_set_response_timeout(ctx, &response_timeout);
			*/
			connected = true;
			return true;
		}


	void Bus::ADDmBD()
	{
		 unsigned short id,versionMajor,versionMinor;
		 char Name[20];

		 memcpy(Name,tab_reg,20);
		 versionMajor= tab_reg[10];
		 versionMinor= tab_reg[11];
		 id= tab_reg[12];
		 //printf("\nDevice Name   : %s",Name);
		 //printf("\nDevice Version: %d.%d",versionMajor,versionMinor);
		 //printf("\nDevice ID     : %d\n",id);

		 // CHECK IF NAME & VERSION IS A MARELAB mBD
		 string name;
		 name.assign(Name);
		 string vuuid;
		 vuuid.assign("A1234");

		 if (	name.compare("marelab mBD") == 0)
		 {
			 MBD mBD(name ,versionMajor,versionMinor,id,ctx,vuuid);
			 mBD_List.push_back(mBD);
			// MLOG::log("Bus: added Device ["+name+"] with BusId "+i2str(id)+ "to marelab system ...",__LINE__,__FILE__);
		 }
		 else
		 {
			// MLOG::log("Bus: try to add Device ["+i2str(id)+"] ... but thats no marelab device!...",__LINE__,__FILE__);
		 }

	}

	void Bus::ScanBus()
	{
	  int i,ii;
	  int con;
	  // Scan the bus for members
	  mBD_List.clear();
	  if (ctx==NULL)
		  cout << "Bus: BUS IS NOT CONNECTED..." << endl;
		  //MLOG::log("Bus: BUS IS NOT CONNECTED..." ,__LINE__,__FILE__);
	  else
	  {
		  for (ii=1;ii <= MAX_BUS_MEMBER; ii++)
		  {
			  con = modbus_set_slave(ctx, ii);
			  cout << con;
			  rc = modbus_read_registers(ctx, 0, 13, tab_reg);
			  if (rc == -1)
			  {
				  cout << "";
				 // MLOG::log("Bus: DEV: "+i2str(ii)+" "+ modbus_strerror(errno),__LINE__,__FILE__);
			  }
			  else
			  {
				  ADDmBD();
				  //for (i=0; i < rc; i++){
				 //MLOG::log("Bus: reg["+i2str(tab_reg[i])+"]="+i2str(tab_reg[i]),__LINE__,__FILE__);
			 //}
		 }
	  }
	  DumpMDB_LIST();
	  }
	}

	// Iterarts throu the Device List and finds the
	// device if registered
	MBD* Bus::GetMBD(int deviceNo)
	{
		MBD *mBD=NULL;

		for(vector<MBD>::iterator it = mBD_List.begin(); it != mBD_List.end(); ++it) {
			if (it->MBD_getAdress() == deviceNo )
				mBD = it.base();
		}
		return mBD;
	}


	string Bus::get_driver(const string& tty) {
	    struct stat st;
	    string devicedir = tty;

	    // Append '/device' to the tty-path
	    devicedir += "/device";

	    // Stat the devicedir and handle it if it is a symlink
	    if (lstat(devicedir.c_str(), &st)==0 && S_ISLNK(st.st_mode)) {
	        char buffer[1024];
	        memset(buffer, 0, sizeof(buffer));

	        // Append '/driver' and return basename of the target
	        devicedir += "/driver";

	        if (readlink(devicedir.c_str(), buffer, sizeof(buffer)) > 0)
	            return basename(buffer);
	    }
	    return "";
	}

	void Bus::register_comport( list<string>& comList, list<string>& comList8250, const string& dir) {
	    // Get the driver the device is using
	    string driver = get_driver(dir);

	    // Skip devices without a driver
	    if (driver.size() > 0) {
	        string devfile = string("/dev/") + basename(dir.c_str());

	        // Put serial8250-devices in a seperate list
	        if (driver == "serial8250") {
	            comList8250.push_back(devfile);
	        } else
	            comList.push_back(devfile);
	    }
	}

	void Bus::probe_serial8250_comports(list<string>& comList, list<string> comList8250) {
	    struct serial_struct serinfo;
	    list<string>::iterator it = comList8250.begin();

	    // Iterate over all serial8250-devices
	    while (it != comList8250.end()) {

	        // Try to open the device
	        int fd = open((*it).c_str(), O_RDWR | O_NONBLOCK | O_NOCTTY);

	        if (fd >= 0) {
	            // Get serial_info
	            if (ioctl(fd, TIOCGSERIAL, &serinfo)==0) {
	                // If device type is no PORT_UNKNOWN we accept the port
	                if (serinfo.type != PORT_UNKNOWN)
	                    comList.push_back(*it);
	            }
	            close(fd);
	        }
	        it ++;
	    }
	}

	list<string> Bus::getComList() {
	    int n;
	    struct dirent **namelist;
	    list<string> comList;
	    list<string> comList8250;
	    const char* sysdir = "/sys/class/tty/";

	    // Scan through /sys/class/tty - it contains all tty-devices in the system
	    n = scandir(sysdir, &namelist, NULL, NULL);
	    if (n < 0)
	        perror("scandir");
	    else {
	        while (n--) {
	            if (strcmp(namelist[n]->d_name,"..") && strcmp(namelist[n]->d_name,".")) {

	                // Construct full absolute file path
	                string devicedir = sysdir;
	                devicedir += namelist[n]->d_name;

	                // Register the device
	                register_comport(comList, comList8250, devicedir);
	            }
	            free(namelist[n]);
	        }
	        free(namelist);
	    }

	    // Only non-serial8250 has been added to comList without any further testing
	    // serial8250-devices must be probe to check for validity
	    probe_serial8250_comports(comList, comList8250);

	    // Return the lsit of detected comports
	    return comList;
	}


	void Bus::Serialize( Json::Value& root )
	{
	    // serialize this object & these object values
		//vector <MBD> mBD_List;
		//string 	comport;
		//int 	baud;
		//int 	databits;
		//char 	parity;
		//int 	stopbits;



		int size = mBD_List.size();
		Json::Value obj_value(Json::objectValue);
		obj_value["BUS"]["PORT"] 		= this->comport;
		obj_value["BUS"]["BAUD"] 		= this->baud;
		obj_value["BUS"]["DATABITS"]	= this->databits;
		obj_value["BUS"]["PARITY"] 		= this->parity;
		obj_value["BUS"]["STOPBITS"] 	= this->stopbits;
		root["BUS"].append(obj_value);
		obj_value.clear();
		for(int i=0; i<size; i++)
		{
			obj_value["MBD"]["ADRESSE"] 		= mBD_List[i].MBD_getAdress();
			obj_value["MBD"]["NAME"] 			= mBD_List[i].MBD_getName();
			obj_value["MBD"]["VERSIOMAJOR"] 	= mBD_List[i].MBD_getMajorVersion();
			obj_value["MBD"]["VERSIOMINOR"] 	= mBD_List[i].MBD_getMinorVersion();
			obj_value["MBD"]["UUID"] 			= mBD_List[i].MBD_getUUID();
			root["MBD"].append(obj_value);
			obj_value.clear();
		}
	}

	void Bus::Deserialize( Json::Value& root){
		// Delete of the registry
		mBD_List.clear();

		int count = root.get("BUS", "").size();
		//Read in the BUS Object
		for (int i = 0; i < count; i++) {
			this->baud = root.get("BUS", "")[i].get("BUS", "").get("BAUD", "").asInt();
			this->comport = root.get("BUS", "")[i].get("BUS", "").get("PORT", "").asString();
			this->databits = root.get("BUS", "")[i].get("BUS", "").get("DATABITS", "").asInt();
			this->parity = root.get("BUS", "")[i].get("BUS", "").get("PARITY", "").asCString()[0];
			this->stopbits = root.get("BUS", "")[i].get("BUS", "").get("STOPBITS", "").asInt();
		}

		// Now we got the port data we try to create a modbus device to have the handle and to add
		// it to the MBD List Objects
		if (ConnectBus()){
			//Read in the MBD Object
			int count = root.get("MBD", "").size();
			string vuuid = "1234";
			for (int i = 0; i < count; i++) {
				MBD mBD(root.get("MBD", "")[i].get("MBD", "").get("NAME", "").asString(),
						root.get("MBD", "")[i].get("MBD", "").get("VERSIOMAJOR", "").asInt(),
						root.get("MBD", "")[i].get("MBD", "").get("VERSIOMINOR", "").asInt(),
						root.get("MBD", "")[i].get("MBD", "").get("ADRESSE", "").asInt(),
						ctx,
						vuuid);
						mBD_List.push_back(mBD);
			}
			//MLOG::log(DumpMDB_LIST(),__LINE__,__FILE__);
		}
		else{
			cout << "Config Error: Can't create MBD because Bus Interface is not ready..." << endl;
		}
	}

	string Bus::DumpMDB_LIST(){
		string ret;
		int size = mBD_List.size();
		ret = "--Dump MBD LIST-------------------------------------\n";
		for(int i=0; i<size; i++)
		{
			//ret = ret+ "Adress     :" + i2str(mBD_List[i].MBD_getAdress())+"\n";
			//ret = ret+ "Major      :" + i2str(mBD_List[i].MBD_getMajorVersion())+"\n";
			//ret = ret+ "Minor      :" + i2str(mBD_List[i].MBD_getMinorVersion())+"\n";
			//ret = ret+ "Name       :" + mBD_List[i].MBD_getName()+"\n";
			//ret = ret+ "UUID       :" + mBD_List[i].MBD_getUUID()+"\n";
			//ret = ret+ "------------------------------------------------\n";
		}
		return ret;
	}

	void Bus::SerializeAjax( Json::Value& root){

	}


}
