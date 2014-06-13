/*
 * PCF8574.h
 *
 *  Created on: 26.03.2013
 *      Author: marelab
 */

#ifndef PCF8574_H_
#define PCF8574_H_
#include <stdio.h>
#include <inttypes.h>
#include "I2C.h"
#include "LConnection.h"

using namespace std;
class PCF8574 :public LConnection{
private:
	uint8_t PCF8574_STATUS;
	I2C *i2c;
public:
	PCF8574(string bus, uint8_t adress);
	virtual ~PCF8574();
	void set(uint8_t setbit, bool onoff); // Sets a bit and leaves others unchanged
	vector <Connector*> getConnectionList();
};

#endif /* PCF8574_H_ */
