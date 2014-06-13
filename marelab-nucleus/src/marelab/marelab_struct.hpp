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

#ifndef MARELAB_STRUCT_H
#define MARELAB_STRUCT_H


#include <iostream>
#include <cstring>   // Required by strcpy()
#include <cstdlib>   // Required by malloc()
#include <vector>

#include "../json/json.h"
#include "IJsonSerializable.h"
#include "CJsonSerializer.h"

#define TIMERSTORECOUNT 48


using namespace std;
/*
 * Timer stucts
 *
 */
class LedString
{
private:
    int LD_number;
    std::string LD_name;
    int LD_TimeArray[48];
    int LD_i2c_channel;
public:
    LedString(void);
    int getLdI2cChannel() const;
    std::string getLdName() const;
    int getLdNumber() const;
    int *getLdTimeArray();
    void setLdI2cChannel(int ldI2cChannel);
    void setLdName(std::string ldName);
    void setLdNumber(int ldNumber);
    void setLdTimeArray(int no, int value);
    void printLedString();
};
#endif
