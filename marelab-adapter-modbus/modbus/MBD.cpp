/*
 * MBD.cpp
 *
 *  Created on: Nov 3, 2012
 *      Author: marelab
 */
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
#include "MBD.h"

namespace std
{


MBD::MBD(string device_name,int dversion_major,int dversion_minor,int dadress,modbus_t *dctx,string muuid)
  {
  	name.assign(device_name);
  	version_major = dversion_major;
  	version_minor = dversion_minor;
  	adress = dadress;
  	mADRUUID.assign(muuid);
  	ctx = dctx;
  }

  unsigned short MBD::MBD_GetDimmValues(int channel)
  {
  	 uint16_t tab_reg[128];
  	 modbus_read_registers(ctx,20, 8, tab_reg);
  	 return tab_reg[channel];
  }

  unsigned short MBD::MBD_SetDimmValues(int channel,int dimm_value)
  {
      	 int ret;
      	 channel = channel + 20;
      	 ret = modbus_write_register(ctx, (unsigned short)channel,(unsigned short) dimm_value);
      	 return ret;
  }

	int MBD::MBD_getAdress(){
		return adress;
	}



	int MBD::MBD_getMajorVersion(){
		return version_major;
	}

	int MBD::MBD_getMinorVersion(){
		return version_minor;
	}

	void MBD::MBD_setAdress(int adress){
		int ret;
		ret = modbus_write_register(ctx, (unsigned short)MODBUS_ADRESS,(unsigned short) adress);
	}

   string MBD::MBD_getName() {
		return name;
	}

	string MBD::MBD_getUUID(){
			return mADRUUID;
	}

}
