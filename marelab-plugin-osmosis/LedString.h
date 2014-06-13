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

#ifndef LEDSTRING_H_
#define LEDSTRING_H_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <syslog.h>
#include <vector>
#include <string>
#include <cstdio>
#include <vector>

#include "../marelab-nucleus/src/marelab/ipccom.h"

#define  DAY_SENSE_POINT   48
/*
 * LedString
 *
 *
 *
 *
 *
 */
class LedString {
private:
	int LD_i2c_channel;
	std::string LD_name;
	int LD_number;
	int LD_TimeArray[DAY_SENSE_POINT];
	std::string ChartColor;				// Color of String in the chart

public:
	LedString(void);
	int getLdI2cChannel() const;
	std::string getLdName() const;
	int getLdNumber() const;
	int* getLdTimeArray();
	std::string getChartColor();

	void setLdI2cChannel(int ldI2cChannel);
	void setLdName(std::string ldName);
	void setLdNumber(int ldNumber);
	void setLdTimeArray(int no, int value);
	void setChartColor(std::string color);
	void printLedString();


};

#endif /* LEDSTRING_H_ */
