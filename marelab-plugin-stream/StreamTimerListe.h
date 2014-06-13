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

#ifndef LEDTIMERLISTE_H_
#define LEDTIMERLISTE_H_
#include <iostream>
#include <cstring>   // Required by strcpy()
#include <cstdlib>   // Required by malloc()
#include <vector>

#include "json/json.h"
#include "IJsonSerializable.h"
#include "CJsonSerializer.h"
#include "StreamDevice.h"

#define TIMERSTORECOUNT 48

namespace std {

class StreamTimerListe  : public IJsonSerializable
{
private:
	vector<StreamDevice> LD_StreamListe;
	vector<int> lastPower;
	string filestore;

	 int FOOD_DELAY;								// Time in Sec that the streams are in food mode
	 int FOOD_MIN_POWER;							// Amount of stream Power during food mode normal 0;


public:
	StreamTimerListe(void){};
    ~StreamTimerListe( void );
    virtual void Serialize( Json::Value& root );
    virtual void Deserialize( Json::Value& root);
    virtual void SerializeAjax( Json::Value& root);
    void SetFileStore(string fileStore);
    void SaveFile();
    void SetFoodDelay(int fooddelay);
    int GetFoodDelay();
    void SetFoodPower(int foodpower);
    int GetFoodPower();
	void AddStream(StreamDevice streamdevice);
	void AddChangeStream(string stream_number, string stream_channel, string stream_name, string stream_intensity,string chartcolor);
	vector<string> split(const string& strValue, char separator);
	int DelStream(int stream_number);
	int CountStream();
	void printStreamListe();
	void Clear();
	StreamDevice* GetStream(int stream_number);
	StreamDevice* GetStreamArrayNo(int stream_number);
	int GetPowerValue(int stream_number,tm *time2Check);
};

} /* namespace std */
#endif /* LEDTIMERLISTE_H_ */
