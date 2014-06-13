/*
 * PCF8574.cpp
 *
 *  Created on: 26.03.2013
 *      Author: marelab
 */

#include "PCF8574.h"
#include "LConnection.h"
using namespace std;

PCF8574::PCF8574(string bus, uint8_t adress) {
	/* Define all Connectors of Hardware the I2C system can offer to logic.
	 * This is the second part of the configured logic-physics. Every logic
	 * connector can be connected to a suitable physics connector. This
	 * is stored in the conf file and the base setup how a logic can access
	 * a physical connection.
	 */
	 Connector *connector;
	 /*
	  * Define MAX520
	  * DA ports 4
	  */
	 connector= new Connector(IO,"PCF8574 IO-0",BI,"");
	 this->Add(connector);
	 connector= new Connector(IO,"PCF8574 IO-1",BI,"");
	 this->Add(connector);
	 connector= new Connector(IO,"PCF8574 IO-2",BI,"");
	 this->Add(connector);
	 connector= new Connector(IO,"PCF8574 IO-3",BI,"");
	 this->Add(connector);
	 connector= new Connector(IO,"PCF8574 IO-4",BI,"");
	 this->Add(connector);
	 connector= new Connector(IO,"PCF8574 IO-5",BI,"");
	 this->Add(connector);
	 connector= new Connector(IO,"PCF8574 IO-6",BI,"");
	 this->Add(connector);
	 connector= new Connector(IO,"PCF8574 IO-7",BI,"");
	 this->Add(connector);

	PCF8574_STATUS = 0;   // All IO Pins OFF
	i2c = new I2C(bus,adress);
	i2c->openfd();
	i2c->setI2CAdr(adress);
	i2c->openfd();
	i2c->write_byte(adress,PCF8574_STATUS);
	i2c->closefd();

}

PCF8574::~PCF8574() {
	// TODO Auto-generated destructor stub
	delete i2c;
}

void PCF8574::set(uint8_t setbit, bool onoff){
		if (onoff == true){
			PCF8574_STATUS &= ~(1 << setbit);
		}
		else {
			PCF8574_STATUS |= (1 << setbit);
		}
		i2c->openfd();
		i2c->setI2CAdr(0x20);
		i2c->openfd();
		i2c->write_byte(0x20,PCF8574_STATUS);
		i2c->closefd();
}

vector <Connector*> PCF8574::getConnectionList(){
  return this->connectorList;
}
