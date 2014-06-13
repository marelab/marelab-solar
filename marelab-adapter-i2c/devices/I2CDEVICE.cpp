/*
 * I2CDEVICE.cpp
 *
 *  Created on: 26.03.2013
 *      Author: marelab
 */
#include <string>
#include "I2CDEVICE.h"
#include "LConnection.h"

/*
 * Set all i2c device handling infos
 */
I2C_DEVICE::I2C_DEVICE(	string vdevice_name,uint8_t vdevice_adr,string vi2c_bus,int vdevice_no,void *vdevice_class ) {
	device_name 	= vdevice_name;
	device_adr 		= vdevice_adr;
	i2c_bus			= vi2c_bus;
	device_no 		= vdevice_no;
	device_class	= vdevice_class;
}

I2C_DEVICE::~I2C_DEVICE() {
	// TODO Auto-generated destructor stub
	cout << "I2C_Device:" << device_name << " Destructor..." << endl;
	delete (LConnection*)device_class;
}

/*
 * Save the DEVICE Info
 */
void I2C_DEVICE::Serialize( Json::Value& root ){

}
/*
 * Read the DEVICE Info
 */
void I2C_DEVICE::Deserialize( Json::Value& root){

}

void I2C_DEVICE::SerializeAjax( Json::Value& root ){

}
