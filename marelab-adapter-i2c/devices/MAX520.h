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

#ifndef MAX520_H_
#define MAX520_H_

#include <stdio.h>
#include <inttypes.h>
#include <vector>
#include "I2C.h"
#include "LConnection.h"
#include "../json/json.h"


class MAX520 : public LConnection{
private:
	uint8_t MAX520_STATUS;
	I2C *i2c;

public:
	MAX520(string bus, uint8_t adress);
	~MAX520();
	void setDimChannel(int channel, int value);
	vector <Connector*> getConnectionList();
};
#endif /* MAX520_H_ */
