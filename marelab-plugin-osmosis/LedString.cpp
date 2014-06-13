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

#include <cstdio>
#include <vector>
#include "LedString.h"

#define c_fileversion "0.1"

int LedString::getLdI2cChannel() const
{
    return LD_i2c_channel;
}

std::string LedString::getLdName() const
{
    return LD_name;
}

int LedString::getLdNumber() const
{
    return LD_number;
}

int* LedString::getLdTimeArray()
{
    return LD_TimeArray;
}

void LedString::setLdI2cChannel(int ldI2cChannel)
{
    LD_i2c_channel = ldI2cChannel;
}

void LedString::setLdName(std::string ldName)
{
    LD_name = ldName;
}

void LedString::setLdNumber(int ldNumber)
{
    LD_number = ldNumber;
}

void LedString::setLdTimeArray(int no, int value)
{
	LD_TimeArray[no]=value;
}

void LedString::printLedString()
{
	printf("\nColor: %s LedString: %i %s I2C: %i Time/Value:\n",ChartColor.c_str(),LD_number,LD_name.c_str(),LD_i2c_channel);
	for (int i=0; i< DAY_SENSE_POINT;i++)
		printf("%i ",LD_TimeArray[i]);

    //int ;
}

LedString::LedString(void)
{
	for (int i=0; i < DAY_SENSE_POINT;i++)
		LD_TimeArray[i]=0;
}

std::string LedString::getChartColor(){
	return ChartColor;
}

void LedString::setChartColor(std::string color){
	ChartColor = color;
}




