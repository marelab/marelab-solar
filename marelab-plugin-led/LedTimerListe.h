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
#include "LedString.h"

#define TIMERSTORECOUNT 48

namespace std {

class LedTimerListe  : public IJsonSerializable
{
private:
	vector<LedString> LD_StringListe;

	string filestore;

public:
	vector<int> lastPower;
	LedTimerListe(void){};
    ~LedTimerListe( void );
    virtual void Serialize( Json::Value& root );
    virtual void Deserialize( Json::Value& root);
    virtual void SerializeAjax( Json::Value& root);
    void SetFileStore(string fileStore);
    void SaveFile();
	void AddLed(LedString ledstring);
	void AddChangeLed(string led_number, string led_channel, string led_name, string led_intensity,string chartcolor,string uuid);
	vector<string> split(const string& strValue, char separator);
	int DelLed(int led_number);
	int CountLed();
	void printLedListe();
	void Clear();
	LedString* GetLed(int led_number);
	LedString* GetLedArrayNo(int led_number);
	int GetPowerValue(int led_number,tm *time2Check);
	int SetLastPowerValue(int led_number,tm *time2Check,int power);
};

} /* namespace std */
#endif /* LEDTIMERLISTE_H_ */
