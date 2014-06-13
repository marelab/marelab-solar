/*
 * MBD.hpp
 *
 *  Created on: Nov 3, 2012
 *      Author: marelab
 */

#ifndef MBD_HPP_
#define MBD_HPP_


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

#include "modbus.h"

namespace std
{

class MBD
{
private:
	 string name;
	 int version_major;
	 int version_minor;
     int adress;
     string mADRUUID;
     modbus_t *ctx;
     unsigned short dimmer[7];

public:
    MBD(string device_name,int dversion_major,int dversion_minor,int dadress,modbus_t *dctx,string uuid);

    unsigned short MBD_GetDimmValues(int channel);

    unsigned short MBD_SetDimmValues(int channel,int dimm_value);

	int MBD_getAdress();

	int MBD_getMajorVersion();

	int MBD_getMinorVersion();

	void MBD_setAdress(int adress);

	string MBD_getUUID();

	string MBD_getName();
};
}
#endif /* MBD_HPP_ */
