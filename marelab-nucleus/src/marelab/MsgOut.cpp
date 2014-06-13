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

#include "MsgOut.h"

namespace std {

MsgOut::MsgOut() {
	// TODO Auto-generated constructor stub

}

MsgOut::~MsgOut() {
	// TODO Auto-generated destructor stub
}



void MsgOut::Serialize( Json::Value& root )
{
	/*
   // serialize primitives
	int size = LD_StringListe.size();
	Json::Value timer_value(Json::arrayValue); // []
	//arr_value.append("Test1");
	//arr_value.append("Test2");
	Json::Value obj_value(Json::objectValue);
	for(int i=0; i<size; i++)
	{
		obj_value["LedString"]["LDNAME"] = LD_StringListe[i].getLdName();
		obj_value["LedString"]["LDNUMBER"] = LD_StringListe[i].getLdNumber();
		obj_value["LedString"]["LDI2CCHANNEL"] = LD_StringListe[i].getLdI2cChannel();
		for (int ii=0;ii<TIMERSTORECOUNT;ii++)
		{
			obj_value["LedString"]["LDTIMEARRAY"].append(LD_StringListe[i].getLdTimeArray()[ii]);
		}
		root["LedListe"].append(obj_value);
		obj_value.clear();
		obj_value["LedString"]["LDTIMEARRAY"].clear();
	}
	// version tag into stream
	root["version"] = c_fileversion;
	*/
}


void MsgOut::Deserialize( Json::Value& root )
{
	/*
	string version =  root.get("version", "").asString();

	// Delete of the list
	this->Clear();

	int countLedLeisten = root.get("LedListe", "").size();
	cout << "listen  size =" << countLedLeisten;
	for(int i=0; i<countLedLeisten; i++)
	{
		LedString ledstring;
		//cout << root.get("LedListe", "")[i].get("LedString","").get("LDNAME","").asString() << "\n";
		ledstring.setLdName(root.get("LedListe", "")[i].get("LedString","").get("LDNAME","").asString());
		ledstring.setLdI2cChannel(root.get("LedListe", "")[i].get("LedString","").get("LDI2CCHANNEL","").asInt());
		ledstring.setLdNumber(root.get("LedListe", "")[i].get("LedString","").get("LDNUMBER","").asInt());
		for (int ii=0;ii<TIMERSTORECOUNT;ii++)
		{
			ledstring.setLdTimeArray( ii,root.get("LedListe", "")[i].get("LedString","").get("LDTIMEARRAY","")[ii].asInt());
		}
		this->AddLed(ledstring);
	}

   m_nTestInt = root.get("LedString",0).asInt();
   m_fTestFloat = root.get("LedString", 0.0).asDouble();
   m_TestString = root.get("LedString", "").asString();
   m_bTestBool = root.get("LedString", false).asBool();
*/
}



} /* namespace std */
