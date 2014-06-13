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
 * Name        : I2C.h
 * Author      : Georgi Todorov
 * Version     :
 * Created on  : Dec 30, 2012
 *
 * Copyright © 2012 Georgi Todorov  <terahz@geodar.com>
 */

#ifndef I2C_H_
#define I2C_H_
#include <inttypes.h>
#include <string>
using namespace std;

#define BUFFER_SIZE 0x01  //1 byte buffer


class I2C {
public:
	I2C();
	I2C(string bus, int adr);
	virtual ~I2C();
	uint8_t dataBuffer[BUFFER_SIZE];
	uint8_t read_byte(uint8_t);
	uint8_t write_byte(uint8_t, uint8_t);
	string getI2CBus();
	void   setI2CBus(string busname);
	uint8_t    getI2CAdr();
	void   setI2CAdr(uint8_t);
	void closefd();
	void openfd();
private:
	int _i2caddr;
	//int _i2cbus;
	string _i2cbus;
	//string busfile;
	int fd;
};

#endif /* I2C_H_ */
