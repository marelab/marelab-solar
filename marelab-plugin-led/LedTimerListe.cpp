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

#include "LedTimerListe.h"

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <errno.h>
#include <pwd.h>
#include <signal.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cctype>
#include <cwctype>
//#include "../marelab-nucleus/src/marelab/mlog.h"

#include "LedString.h"
#include "LedTimerListe.h"
//#include "plugin-config.h"

#define c_fileversion "0.1"

namespace std {

void LedTimerListe::AddLed(LedString ledstring)
{
	LD_StringListe.push_back(ledstring);
	lastPower.push_back(-1);
}


LedTimerListe::~LedTimerListe()
{
	cout << "LedTimerListe Destructor..." << endl;
	/*
	 int sizeofLedListe  = this->CountLed();
	for(int i=0; i<sizeofLedListe; i++)
	{
		delete this->GetLed(i);
	}
	*/
}


/*
 * Add the file where to store the list
 */
void LedTimerListe::SetFileStore(string fileStore)
{
	filestore = fileStore;
}

void LedTimerListe::SaveFile()
{
	Json::Value rootin;
	string output;

	std::ofstream out(filestore.c_str());
	CJsonSerializer::Serialize(this, output);
	out << output;
	out.close();
}


/*
 * Add or Changes a LED Channel
 */
void LedTimerListe::AddChangeLed(string led_number, string led_channel, string led_name, string led_intensity, string chartcolor,string uuid)
{
	int ledno;
	LedString *ls;
	// Find the channel object
	string compare_Led="_empty";
	cout << "AddChangeLed No:" << led_number << endl;
	if (led_number == compare_Led )
	{
		cout << "LedTimerListe::AddChangeLed ADD NEW LED" << endl;
	    // CALC the ID
		int size = LD_StringListe.size();
		ledno =  LD_StringListe[size-1].getLdNumber()+1;

		//ledno = CountLed()+1;
		ls = new LedString();
		ls->setLdNumber(ledno);
		ls->setLdName(led_name);
		ls->setLdI2cChannel(atoi(led_channel.c_str()));
		ls->setChartColor(chartcolor);
		ls->setUUID(uuid);
		this->AddLed(*ls);
		// LED intensität umsetzen
		led_intensity = "[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]";
		//ls->LD_TimeArray = led_intensity;
	}
	else
	{
		cout << "LedTimerListe::AddChangeLed CHANGED OLD LED" << endl;
		ledno = atoi(led_number.c_str());
		cout << "LED-NO to change:" << ledno << endl;
		ls = GetLed(ledno);
		ls->setLdName(led_name);
		ls->setLdI2cChannel(atoi(led_channel.c_str()));
		ls->setChartColor(chartcolor);
		ls->setLdNumber(ledno);
		ls->setUUID(uuid);
	}



	// create Array from string led_intensity
	int leng = led_intensity.length() - 2;
	led_intensity = led_intensity.substr(1, leng);

	vector<string> str_arr;
	str_arr = split(led_intensity, ',');

	for (unsigned int xx = 0; xx < str_arr.size(); xx++){
		ls->setLdTimeArray(xx,atoi(str_arr.at(xx).c_str()));
	}
	//printLedListe();
	//SaveFile();		//Save the LedTimerList to a file
}

int LedTimerListe::DelLed(int led_number)
{
	LedString *ledstring;
	int size = LD_StringListe.size();
	for(int i=0; i<size; i++){
		if (LD_StringListe[i].getLdNumber()==led_number){
			ledstring = &LD_StringListe.operator [](i);
			cout << "DELETING LED NO :" << ledstring->getLdNumber()<< endl;
			LD_StringListe.erase(LD_StringListe.begin()+(i));
			lastPower.erase(lastPower.begin()+i);
		}
	}
	return 0;
}

int LedTimerListe::CountLed()
{
	return LD_StringListe.size();
}

LedString* LedTimerListe::GetLedArrayNo(int led_number)
{
	return &LD_StringListe.operator [](led_number);
}

LedString* LedTimerListe::GetLed(int led_number)
{
	LedString *ledstring=NULL;
	int size = LD_StringListe.size();
	for (int i = 0; i < size; i++) {
		if (LD_StringListe[i].getLdNumber() == led_number) {
			cout << "Return ldid: " << LD_StringListe[i].getLdNumber() << endl;
			ledstring = &LD_StringListe.operator [](i);
		}
	}
	//return &LD_StringListe.operator [](led_number);
	return ledstring;
}

void LedTimerListe::Clear()
{
	LD_StringListe.clear();
}

void LedTimerListe::SerializeAjax( Json::Value& root )
{
   // serialize primitives
	int size = LD_StringListe.size();

	Json::Value timer_value(Json::arrayValue); // []
	Json::Value row(Json::arrayValue);
	Json::Value rowitem(Json::arrayValue);
	Json::Value rowdata(Json::arrayValue);
	Json::Value ledvalues(Json::arrayValue);
	Json::Value zeile(Json::objectValue);
	//arr_value.append("Test1");
	//arr_value.append("Test2");
	Json::Value obj_value(Json::objectValue);
	for(int i=0; i<size; i++)
	{
		for (int ii=0;ii<TIMERSTORECOUNT;ii++)
		{
			ledvalues.append(LD_StringListe[i].getLdTimeArray()[ii]);
		}
		zeile["id"]   = LD_StringListe[i].getLdNumber();
		rowitem.append(LD_StringListe[i].getLdNumber());
		rowitem.append(LD_StringListe[i].getLdName());
		rowitem.append(LD_StringListe[i].getLdI2cChannel());
		rowitem.append(ledvalues);
		rowitem.append(LD_StringListe[i].getChartColor());
		rowitem.append(LD_StringListe[i].getUUID());
		zeile["cell"] = rowitem;
		root["LedListe"].append(zeile);
		ledvalues.clear();
		rowitem.clear();
	}
	// version tag into LedDimmer Plugin
	root["version"] = "2.0";
}

void LedTimerListe::Serialize( Json::Value& root )
{
   // serialize primitives
	unsigned int size = LD_StringListe.size();
	//Json::Value timer_value(Json::arrayValue); // []
	//arr_value.append("Test1");
	//arr_value.append("Test2");
	Json::Value obj_value(Json::objectValue);
	for(unsigned int i=0; i<size; i++)
	{
		obj_value["LedString"]["LDNAME"] = LD_StringListe[i].getLdName();
		obj_value["LedString"]["LDNUMBER"] = LD_StringListe[i].getLdNumber();
		obj_value["LedString"]["LDI2CCHANNEL"] = LD_StringListe[i].getLdI2cChannel();
		for (int ii=0;ii<TIMERSTORECOUNT;ii++)
		{
			obj_value["LedString"]["LDTIMEARRAY"].append(LD_StringListe[i].getLdTimeArray()[ii]);
		}
		obj_value["LedString"]["CHARTCOLOR"]=  LD_StringListe[i].getChartColor();
		obj_value["LedString"]["UUID"]=  LD_StringListe[i].getUUID();
		root["LedListe"].append(obj_value);
		obj_value.clear();
		obj_value["LedString"]["LDTIMEARRAY"].clear();
	}
	// version tag into stream
	root["version"] = c_fileversion;
}

void LedTimerListe::printLedListe()
{
	int sizeofLedListe  = this->CountLed();
	for(int i=0; i<sizeofLedListe; i++)
	{
		this->GetLed(i)->printLedString();
	}
}


void LedTimerListe::Deserialize( Json::Value& root )
{

	Json::Value config;
	string version =  root.get("version", "").asString();
	// Delete of the list
	this->Clear();
	unsigned int a = 0;
	config = root["LedDimmer"][a];
	unsigned int countLedLeisten = config.get("LedListe", "").size();
	//string ledchannelcout = i2str(countLedLeisten);
	//MLOG::log("PLUGIN LED: Count of Dim Channels = "+ledchannelcout,__LINE__,__FILE__);
	cout << "LED: Count of Dim Channels =" << countLedLeisten << endl;
	for(unsigned int i=0; i<countLedLeisten; i++)
	{
		LedString ledstring;
		ledstring.setLdName(config.get("LedListe", "")[i].get("LedString","").get("LDNAME","").asString());
		ledstring.setUUID(config.get("LedListe", "")[i].get("LedString","").get("UUID","").asString());
		ledstring.setLdI2cChannel(config.get("LedListe", "")[i].get("LedString","").get("LDI2CCHANNEL","").asInt());
		ledstring.setLdNumber(config.get("LedListe", "")[i].get("LedString","").get("LDNUMBER","").asInt());
		for (int ii=0;ii<TIMERSTORECOUNT;ii++)
		{
			ledstring.setLdTimeArray( ii,config.get("LedListe", "")[i].get("LedString","").get("LDTIMEARRAY","")[ii].asInt());
		}
		ledstring.setChartColor(config.get("LedListe", "")[i].get("LedString","").get("CHARTCOLOR","").asString());
		cout << "CHARTCOLOR ADD:" << config.get("LedListe", "")[i].get("LedString","").get("CHARTCOLOR","").asString() << endl;
		this->AddLed(ledstring);
	}



}


vector<string> LedTimerListe::split(const string& strValue, char separator)
{
    vector<string> vecstrResult;
    int startpos=0;
    int endpos=0;

    endpos = strValue.find_first_of(separator, startpos);
    while (endpos != -1)
    {
        vecstrResult.push_back(strValue.substr(startpos, endpos-startpos)); // add to vector
        startpos = endpos+1; //jump past sep
        endpos = strValue.find_first_of(separator, startpos); // find next
        if(endpos==-1)
        {
            //lastone, so no 2nd param required to go to end of string
            vecstrResult.push_back(strValue.substr(startpos));
        }
    }

    return vecstrResult;
}

int LedTimerListe::SetLastPowerValue(int led_number,tm *time2Check,int power)
{

	LedString* ledstring = GetLedArrayNo(led_number);

	int hour_now = time2Check->tm_hour;
	int min_now  = time2Check->tm_min;
	int sec_now  = time2Check->tm_sec;


	unsigned int index1;

	index1 = ((unsigned int) ((hour_now*3600)+(min_now*60)+sec_now)/1800);
	//cout << "TimeIndex:" << index1 << endl;

	cout << "Index: "<< index1 <<" Intensity "<<hour_now<<":"<<min_now<<":"<<sec_now<<" "<< ledstring->getLdName() << " New=" << power << "% Old= " << lastPower.operator [](led_number) << "%"<<endl;
	lastPower.operator [](led_number) = power;
	return power;
}


/* Gets the % 0-100 of power for a given time */
int LedTimerListe::GetPowerValue(int led_number,tm *time2Check)
{

	LedString* ledstring = GetLedArrayNo(led_number);

	int hour_now = time2Check->tm_hour;
	int min_now  = time2Check->tm_min;
	int sec_now  = time2Check->tm_sec;

	/* Delta calculation */
	int pvalue1;
	int pvalue2;
	unsigned int index1, index2;

	index1 = ((unsigned int) ((hour_now*3600)+(min_now*60)+sec_now)/1800);
	//cout << "TimeIndex:" << index1 << endl;
	pvalue1 = ledstring->getLdTimeArray()[index1];
	//change from 23:30 -> 00:00
	if (index1 == 47)
		index2 = 0;
	else
		index2 = index1+1;

	pvalue2 = ledstring->getLdTimeArray()[index2];

	//cout << "INTERVAL"<< index1 << " VALUE:" << pvalue1;

	double yy = (pvalue2 - pvalue1);
	double delta = (double)yy/1800;

	unsigned int time2calc = ((hour_now*3600)+(min_now*60)+sec_now) -(index1 * 1800);
	int power = (int)pvalue1+(delta*time2calc);
	if (power != lastPower.operator [](led_number))
	{
		cout << "Index: "<< index1 <<" Intensity "<<hour_now<<":"<<min_now<<":"<<sec_now<<" "<< ledstring->getLdName() << " New=" << power << "% Old= " << lastPower.operator [](led_number) << "%"<<endl;
		lastPower.operator [](led_number) = power;
		return power;
	}
	return -1;
}

} /* namespace std */
