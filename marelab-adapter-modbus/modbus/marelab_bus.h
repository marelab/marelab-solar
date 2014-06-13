/*
 * marelab_bus.h
 *
 *  Created on: Nov 3, 2012
 *      Author: marelab
 */

#ifndef MARELAB_BUS_H_
#define MARELAB_BUS_H_
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
#include "MBD.h"

#include "../globals/json/json.h"
#include "../IJsonSerializable.h"
//#include "../marelab/CJsonSerializer.h"
//#include "../marelab/ConfigRegister.h"

namespace std
{

class Bus  : public IJsonSerializable	{
private:
	modbus_t *ctx;
	uint16_t tab_reg[128];
	vector <MBD> mBD_List;
	int rc;
	string 	comport;
	int 	baud;
	int 	databits;
	string 	parity;
	int 	stopbits;
	bool	connected;

public:
	Bus ();
	~Bus();
	void ADDmBD();
	void ScanBus();
	bool ConnectBus(char *port,int baudrate);
	bool ConnectBus();
	bool isConnected();
	MBD* GetMBD(int deviceNo);

	string get_driver(const string& tty);
	void register_comport( list<string>& comList, list<string>& comList8250, const string& dir);
	void probe_serial8250_comports(list<string>& comList, list<string> comList8250);
	list<string> getComList();
	string DumpMDB_LIST();
	virtual void Serialize( Json::Value& root );
	virtual void Deserialize( Json::Value& root);
	virtual void SerializeAjax( Json::Value& root);



	vector<MBD> getMBdList() const {
		return mBD_List;
	}

	int getBaud() const {
		return baud;
	}

	void setBaud(int baud) {
		this->baud = baud;
	}

	string getComport() const {
		return comport;
	}

	void setComport(string comport) {
		this->comport = comport;
	}

	int getDatabits() const {
		return databits;
	}

	void setDatabits(int databits) {
		this->databits = databits;
	}

	string getParity() const {
		return parity;
	}

	void setParity(char parity) {
		this->parity = parity;
	}

	int getStopbits() const {
		return stopbits;
	}

	void setStopbits(int stopbits) {
		this->stopbits = stopbits;
	}
};
}
#endif /* MARELAB_BUS_H_ */
