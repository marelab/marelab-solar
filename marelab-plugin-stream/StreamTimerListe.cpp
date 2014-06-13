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

#include "StreamTimerListe.h"

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

#include "StreamDevice.h"

//#include "plugin-config.h"

#define c_fileversion "0.1"

namespace std {

void StreamTimerListe::AddStream(StreamDevice streamdevice)
{
	LD_StreamListe.push_back(streamdevice);
	lastPower.push_back(-1);
}


void StreamTimerListe::SetFoodDelay(int fooddelay){
	FOOD_DELAY = fooddelay;
}
int StreamTimerListe::GetFoodDelay(){
	 return FOOD_DELAY;
}
void StreamTimerListe::SetFoodPower(int foodpower){
	 FOOD_MIN_POWER=foodpower;
}
int StreamTimerListe::GetFoodPower(){
	 return FOOD_MIN_POWER;
}

StreamTimerListe::~StreamTimerListe(){
	cout << "StreamTimerListe Destructor..." << endl;
}

	/*
	 int sizeofLedListe  = this->CountLed();
	for(int i=0; i<sizeofLedListe; i++)
	{
		delete this->GetLed(i);
	}
	*/



/*
 * Add the file where to store the list
 */
void StreamTimerListe::SetFileStore(string fileStore)
{
	filestore = fileStore;
}

void StreamTimerListe::SaveFile()
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
void StreamTimerListe::AddChangeStream(string stream_number, string stream_channel, string stream_name, string stream_intensity, string chartcolor)
{
	int streamno;
	StreamDevice *ls;
	// Find the channel object
	string compare_stream="_empty";
	cout << "AddChangeLed No:" << stream_number << endl;
	if (stream_number == compare_stream )
	{
		cout << "Stream Liste::AddChangeSTREAM ADD NEW STREAM" << endl;
	    // CALC the ID
		int size = LD_StreamListe.size();
		streamno =  LD_StreamListe[size-1].getLdNumber()+1;

		//ledno = CountLed()+1;
		ls = new StreamDevice();
		ls->setLdNumber(streamno);
		ls->setLdName(stream_name);
		ls->setLdI2cChannel(atoi(stream_channel.c_str()));
		ls->setChartColor(chartcolor);
		this->AddStream(*ls);
		// LED intensität umsetzen
		stream_intensity = "[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]";
		//ls->LD_TimeArray = led_intensity;
	}
	else
	{
		cout << "STREAMTimerListe::AddChangeSTREAM CHANGED OLD STREAM" << endl;
		streamno = atoi(stream_number.c_str());
		cout << "LED-NO to change:" << streamno << endl;
		ls = GetStream(streamno);
		ls->setLdName(stream_name);
		ls->setLdI2cChannel(atoi(stream_channel.c_str()));
		ls->setChartColor(chartcolor);
		ls->setLdNumber(streamno);
	}



	// create Array from string led_intensity
	int leng = stream_intensity.length() - 2;
	stream_intensity = stream_intensity.substr(1, leng);

	vector<string> str_arr;
	str_arr = split(stream_intensity, ',');

	for (unsigned int xx = 0; xx < str_arr.size(); xx++){
		ls->setLdTimeArray(xx,atoi(str_arr.at(xx).c_str()));
	}
	//printLedListe();
	//SaveFile();		//Save the LedTimerList to a file
}

int StreamTimerListe::DelStream(int stream_number)
{
	StreamDevice *ledstring;
	int size = LD_StreamListe.size();
	for(int i=0; i<size; i++){
		if (LD_StreamListe[i].getLdNumber()==stream_number){
			ledstring = &LD_StreamListe.operator [](i);
			cout << "DELETING LED NO :" << ledstring->getLdNumber()<< endl;
			LD_StreamListe.erase(LD_StreamListe.begin()+(i));
			lastPower.erase(lastPower.begin()+i);
		}
	}
	return 0;
}

int StreamTimerListe::CountStream()
{
	return LD_StreamListe.size();
}

StreamDevice* StreamTimerListe::GetStreamArrayNo(int stream_number)
{
	return &LD_StreamListe.operator [](stream_number);
}

StreamDevice* StreamTimerListe::GetStream(int stream_number)
{
	StreamDevice *streamdevice=NULL;
	int size = LD_StreamListe.size();
	for (int i = 0; i < size; i++) {
		if (LD_StreamListe[i].getLdNumber() == stream_number) {
			cout << "Return ldid: " << LD_StreamListe[i].getLdNumber() << endl;
			streamdevice = &LD_StreamListe.operator [](i);
		}
	}
	//return &LD_StringListe.operator [](led_number);
	return streamdevice;
}

void StreamTimerListe::Clear()
{
	LD_StreamListe.clear();
}

void StreamTimerListe::SerializeAjax( Json::Value& root )
{
   // serialize primitives
	int size = LD_StreamListe.size();

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
			ledvalues.append(LD_StreamListe[i].getLdTimeArray()[ii]);
		}
		zeile["id"]   = LD_StreamListe[i].getLdNumber();
		rowitem.append(LD_StreamListe[i].getLdNumber());
		rowitem.append(LD_StreamListe[i].getLdName());
		rowitem.append(LD_StreamListe[i].getLdI2cChannel());
		rowitem.append(ledvalues);
		rowitem.append(LD_StreamListe[i].getChartColor());
		zeile["cell"] = rowitem;

		root["StreamListe"].append(zeile);
		ledvalues.clear();
		rowitem.clear();
	}
	// version tag into stream
	root["version"] = "2.0";
	root["fooddelay"] = FOOD_DELAY;								// Time in Sec that the streams are in food mode
	root["foodpower"] = FOOD_MIN_POWER;
}

void StreamTimerListe::Serialize( Json::Value& root )
{
   // serialize primitives
	unsigned int size = LD_StreamListe.size();
	//Json::Value timer_value(Json::arrayValue); // []
	//arr_value.append("Test1");
	//arr_value.append("Test2");
	Json::Value obj_value(Json::objectValue);
	for(unsigned int i=0; i<size; i++)
	{
		obj_value["STREAM"]["LDNAME"] = LD_StreamListe[i].getLdName();
		obj_value["STREAM"]["LDNUMBER"] = LD_StreamListe[i].getLdNumber();
		obj_value["STREAM"]["LDI2CCHANNEL"] = LD_StreamListe[i].getLdI2cChannel();
		for (int ii=0;ii<TIMERSTORECOUNT;ii++)
		{
			obj_value["STREAM"]["LDTIMEARRAY"].append(LD_StreamListe[i].getLdTimeArray()[ii]);
		}
		obj_value["STREAM"]["CHARTCOLOR"]=  LD_StreamListe[i].getChartColor();
		root["StreamListe"].append(obj_value);
		obj_value.clear();
		obj_value["STREAM"]["LDTIMEARRAY"].clear();
	}
	// version tag into stream
	root["version"] = c_fileversion;
}

void StreamTimerListe::printStreamListe()
{
	int sizeofLedListe  = this->CountStream();
	for(int i=0; i<sizeofLedListe; i++)
	{
		this->GetStream(i)->printLedString();
	}
}

void StreamTimerListe::Deserialize( Json::Value& root )
{

	Json::Value config;
	string version =  root.get("version", "").asString();
	// Delete of the list
	this->Clear();
	unsigned int a = 0;
	config = root["Stream"][a];
	unsigned int countStreamListe = config.get("StreamListe", "").size();
	//string ledchannelcout = i2str(countLedLeisten);
	//MLOG::log("PLUGIN LED: Count of Dim Channels = "+ledchannelcout,__LINE__,__FILE__);
	cout << "STREAM: Count of Dim Channels =" << countStreamListe << endl;
	for(unsigned int i=0; i<countStreamListe; i++)
	{
		StreamDevice streamDevice;
		streamDevice.setLdName(config.get("StreamListe", "")[i].get("STREAM","").get("LDNAME","").asString());
		streamDevice.setLdI2cChannel(config.get("StreamListe", "")[i].get("STREAM","").get("LDI2CCHANNEL","").asInt());
		streamDevice.setLdNumber(config.get("StreamListe", "")[i].get("STREAM","").get("LDNUMBER","").asInt());
		streamDevice.setUUID(config.get("StreamListe", "")[i].get("STREAM","").get("UUID","").asString());
		for (int ii=0;ii<TIMERSTORECOUNT;ii++)
		{
			streamDevice.setLdTimeArray( ii,config.get("StreamListe", "")[i].get("STREAM","").get("LDTIMEARRAY","")[ii].asInt());
		}
		streamDevice.setChartColor(config.get("StreamListe", "")[i].get("STREAM","").get("CHARTCOLOR","").asString());
		cout << "CHARTCOLOR ADD:" << config.get("StreamListe", "")[i].get("STREAM","").get("CHARTCOLOR","").asString() << endl;
		this->AddStream(streamDevice);
	}



}


vector<string> StreamTimerListe::split(const string& strValue, char separator)
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

/* Gets the % 0-100 of power for a given time */
int StreamTimerListe::GetPowerValue(int stream_number,tm *time2Check)
{

	StreamDevice* ledstring = GetStreamArrayNo(stream_number);

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
	if (power != lastPower.operator [](stream_number))
	{
		cout << "Index: "<< index1 <<" Intensity "<<hour_now<<":"<<min_now<<":"<<sec_now<<" "<< ledstring->getLdName() << " New=" << power << "% Old= " << lastPower.operator [](stream_number) << "%"<<endl;
		lastPower.operator [](stream_number) = power;
		return power;
	}
	return -1;
}

} /* namespace std */
