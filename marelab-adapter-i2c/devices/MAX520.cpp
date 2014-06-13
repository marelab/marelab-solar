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

#include "MAX520.h"
//#include "I2C_Plugin.hpp"
#include "I2C.h"
#include "LConnection.h"

using namespace std ;

MAX520::MAX520(string bus, uint8_t adress) {
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
	 connector= new Connector(DA,"MAX520 DA-1",OUT,"");
	 this->Add(connector);
	 connector= new Connector(DA,"MAX520 DA-2",OUT,"");
	 this->Add(connector);
	 connector= new Connector(DA,"MAX520 DA-3",OUT,"");
	 this->Add(connector);
	 connector= new Connector(DA,"MAX520 DA-4",OUT,"");
	 this->Add(connector);

	 /* Init Status */
	 MAX520_STATUS = 0;   // All IO Pins OFF
	 i2c = new I2C(bus,adress);
	 i2c->openfd();
	 i2c->setI2CAdr(adress);
	 i2c->openfd();
	 i2c->write_byte(adress,MAX520_STATUS);
	 i2c->closefd();
}


MAX520::~MAX520() {
	delete i2c;
}

void MAX520::setDimChannel(int channel, int value){
	float aufloesung = 255;
	float fvalue = (aufloesung / 100)*(float)value;
	int setval = fvalue;
	int i2cadr = i2c->getI2CAdr();
	cout << "DIMM MAX520 I2C BUS    : " << i2c->getI2CBus() << endl;
	cout << "DIMM MAX520 I2C ADRESS : " << i2cadr << endl;
	cout << "DIMM MAX520 I2C CHANNEL: " << channel << endl;
	cout << "DIMM MAX520 I2C VALUE  : " << setval << endl;
	i2c->openfd();
	i2c->write_byte(channel,setval);
	i2c->closefd();
}

vector <Connector*> MAX520::getConnectionList(){
  return this->connectorList;
}
