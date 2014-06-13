/*
 * I2CDEVICE.h
 *
 *  Created on: 26.03.2013
 *      Author: marelab
 */

#ifndef I2CDEVICE_H_
#define I2CDEVICE_H_
#include <inttypes.h>
#include <string>
#include "../json/json.h"
#include "../globals/IJsonSerializable.h"
using namespace std;

class I2C_DEVICE : public IJsonSerializable{
private:
	string device_name;
	uint8_t device_adr;
	string i2c_bus;
	int device_no;
	void *device_class;

public:
	I2C_DEVICE(	string device_name,uint8_t device_adr,string i2c_bus,int device_no,void *device_class );
	virtual ~I2C_DEVICE();
	void Serialize( Json::Value& root );
	void Deserialize( Json::Value& root );
	void SerializeAjax( Json::Value& root );

	uint8_t getDeviceAdr() const {
		return device_adr;
	}

	void setDeviceAdr(uint8_t deviceAdr) {
		device_adr = deviceAdr;
	}

	void* getDeviceClass() const {
		return device_class;
	}

	void setDeviceClass(void* deviceClass) {
		device_class = deviceClass;
	}

	const string& getDeviceName() const {
		return device_name;
	}

	void setDeviceName(const string& deviceName) {
		device_name = deviceName;
	}

	int getDeviceNo() const {
		return device_no;
	}

	void setDeviceNo(int deviceNo) {
		device_no = deviceNo;
	}

	const string& getI2cBus() const {
		return i2c_bus;
	}

	void setI2cBus(const string& i2cBus) {
		i2c_bus = i2cBus;
	}
};

#endif /* I2CDEVICE_H_ */
