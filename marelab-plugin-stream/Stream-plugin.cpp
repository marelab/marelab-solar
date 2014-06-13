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

#ifndef LEDDIMMER_HPP_
#define LEDDIMMER_HPP_

#include <string>
#include <time.h>
#include "StreamDevice.h"
#include "StreamTimerListe.h"
#include "Plugin.hpp"
#include "CJsonSerializer.h"
#include "plugin-config.h"
#include "LConnection.h"
#include "json/json.h"

class Stream : public Plugin, LConnection {
protected:
    StreamTimerListe 	streamListe;				// List of all Streams
    time_t 				foodmode_activated;			// SystemTime when FoodMode was activated
    bool				stopmode_activated;			// All streams have to stop if true



public:
    ~Stream(){
    	cout << "Stream Destructor..." << endl;
    	streamListe.Clear();
    }

    void SetStopMode(bool stop){
    	stopmode_activated = stop;
    }
    void SetFoodTime(time_t t){
    	foodmode_activated = t;
    }

    string getName(){
    	string name = pluginname;
    	return name ;
    }

    string getVersion(){
    	string version = pluginversion;
    	return version;
    }

    string getTypeOfPlugin(){
        	string typeofplugin = TYPE_OF_PLUGIN;
        	return typeofplugin;
    }

    // Returns A JSON string with all compatible
    // Hardware Adapters that can be used with
    // that Logic plugin
    Json::Value hardwareToUse(){
    	ADAPTER_INFO adapter;
    	string retmsg;
    	int i;
    	Json::Value hardware_array(Json::arrayValue);
    	Json::Value obj_value(Json::objectValue);
    	for (i=0; i <= HELPER_ADAPTER_COUNT; i++)
    	{
    		 adapter = COMPATIBLE_ADAPTERS[i];
    		 obj_value["ADAPTER_NAME"] = adapter.AdapterName;
    		 obj_value["ADAPTER_VERSION"] = adapter.AdapterVersion;
    		 hardware_array.append(obj_value);
    	}
        return hardware_array;
    }

   // Save the leddimmer obj to the JSON root var
   // for storing
   void Serialize( Json::Value& root )
    {

	   streamListe.Serialize(root);
	   root["fooddelay"] = streamListe.GetFoodDelay();
	   root["foodpower"] = streamListe.GetFoodPower();
    }


    void Deserialize( Json::Value& root){
    	streamListe.Deserialize(root);
    	cout << root.toStyledString();
    	// Getting food power & timer global vars
    	Json::Value config;
    	unsigned int a = 0;
    	config = root["Stream"][a];
    	streamListe.SetFoodDelay(config.get("fooddelay", "").asInt());
    	streamListe.SetFoodPower(config.get("foodpower","").asInt());
    	/*
    	 * Now we create the connetion List that is needed
    	 * to connect locical IO to phy. IO
    	 */
    	//Connector *UP_CTRL = new Connector(IO,"IO Pin for Motor Up",OUT);
    	//Connector *DOWN_CTRL = new Connector(IO,"IO Pin for Motor DOWN",OUT);
    	//this->Add(UP_CTRL);
    	//this->Add(DOWN_CTRL);
    	Connector *stream_liste;
    	for (int counter=1; counter <=  streamListe.CountStream(); counter++){
    		StreamDevice* streamdevice = streamListe.GetStream(counter);
    		string name = "DA Channel for Stream-" + streamdevice->getLdName();
    	    stream_liste = new Connector(DA,name,OUT,streamdevice->getUUID());
    	    this->Add(stream_liste);
    	}
    	cout << "---- LISTE STREAM CONNETRORS -----" << endl;
    	this->DebugConnectorList();
    	cout << "---- LISTE STREAM CONNETRORS -----" << endl;
    }

    void SerializeAjax( Json::Value& root ){
    	//cout << "test";
    	streamListe.SerializeAjax(root);
    }

    string getConfigHTML(string logic){
    	  return "";
    }

    void work(tm *calltime, Plugin *adapter,Json::Value &LedDimmer){
    	int stream_intensity = 0;
	  	time_t 	tim;				// SystemTime
    	tim=time(NULL);
    	tm *timeNow = localtime(&tim);
    	//cout << "SIZE of streamlist: " << streamListe.CountStream()<< endl;

    	// Finding the configured Hardware Adapter
    	//cout << "CALLED WORK LOGIC : " << this->getName()<< endl;
    	for (int counter=0; counter <  streamListe.CountStream(); counter++){
    		// Ceck if FoodMode activated
    		if ( (foodmode_activated +  streamListe.GetFoodDelay()) > time(NULL)&& (!stopmode_activated))
    		{
    			stream_intensity = streamListe.GetFoodPower();
    			cout << "STREAN FOODMODE for  : " << streamListe.GetFoodDelay()<< " sec. with " << streamListe.GetFoodPower() << "% stream power" << endl;
    		}
    		else if (stopmode_activated){
    			stream_intensity = 0;
    			cout << "STREAM STOPMODE!!!..." << endl;
    		}
    		else
    		{
    			// Calculate the  LED 0-100% Value for a given led string from the graph
    			stream_intensity = streamListe.GetPowerValue(counter,timeNow);
    		}
    		if (stream_intensity != -1){
    			// Send the value to the register of the configured Hardware Adapter
    			// This is just a value transport the hardware adaption is done in the
    			// Hardware Adapter!!!
    			Json::Value jsonStreamPower;
    			jsonStreamPower["LOGIC_PLUGIN_NAME"] = this->getName();
    			jsonStreamPower["DIMMER_STRING_NO"] = counter;
    			jsonStreamPower["DIMMER_CHANNEL"] = streamListe.GetStreamArrayNo(counter)->getLdI2cChannel();
    			jsonStreamPower["DIMMER_VALUE"] = stream_intensity;
    			adapter->work(calltime,this,jsonStreamPower);
    		}
    	}
    	if (stream_intensity != -1)
    	cout << "--------------------" << endl;
  }

  void Command( Plugin *adapter,Json::Value& root ){
	  string parameter = root.get("STREAM","").asString();
	  // A FOOD CYCLE SHOULD BE ACTIVATED
	  if (parameter.compare("FOOD") == 0) {
		  cout <<"STREAM TO FOOD CYCLE ACTIVATED..."<< root.toStyledString() << endl;
		  foodmode_activated=time(NULL);
		  //adapter->Command(adapter,root);
	  }
	  // A STOP COMMAND FOR ALL STREAMS IS SENDED
	  else if (parameter.compare("STOP") == 0) {

		  if (stopmode_activated){
			  SetStopMode(false);
			  cout <<"STREAM ALL START ACTIVAED... "<< root.toStyledString() << endl;
		  }
		  else{
			  SetStopMode(true);
			  cout <<"STREAM ALL STOP ACTIVAED... "<< root.toStyledString() << endl;
		  }
	  }
  }



  void GetConfigAsJSON( string& configval){
	  CJsonSerializer::SerializeAjax(&streamListe, configval);
  }

  /*
   * Getting the parameter for the STREAM that
   * has to be stored out of the JSON request
   * LDI2CHANNEL  	-> CHANNEL for marelab control
   * LDID			-> ID in the internal marelab list
   * LDNAME			-> Name of the LED STRING
   * LDLEDARRAY		-> 48 Values for a 24h day
   */
  void SetConfigAsJSON( Json::Value& para){

	cout << "FROM-PLUGIN-SAVE:"<< endl;
	// Convert recv. JSON led_leiste to a C object
  	string ldid;
  	if (para != Json::Value::null) {
  		if (para["oper"].asString().compare("del") == 0) {
  			// Delete LED Entry
  			string id2del = para["id"].asString();
  			//MLOG::log("SAVELEDROW: DELETE .. " + id2del,__LINE__,__FILE__);
  			cout << "DEL ROW: " << id2del << endl;
  			streamListe.DelStream(atoi(id2del.c_str()));

  		}
  		else if (para["oper"].asString().compare("add") == 0) {
  			cout << "ADD STREAM:" << para.toStyledString() << endl;
  			ldid = para["id"].asString();
  			string ldname = para["LDNAME"].asString();
  			string ldchannel = para["LDI2CCHANNEL"].asString();
  			string intensity = para["LDLEDARRAY"].toStyledString();
  			string chartcolor = para["CHARTCOLOR"].asString();
  			streamListe.AddChangeStream(ldid, ldchannel, ldname, intensity,chartcolor);
  		}
  		else if (para["oper"].asString().compare("edit") == 0) {
				cout << "EDIT STREAM:" << para.toStyledString() << endl;
				ldid = para["id"].asString();
				string ldname = para["LDNAME"].asString();
				string ldchannel = para["LDI2CCHANNEL"].asString();
				string intensity = para["LDLEDARRAY"].toStyledString();
				string chartcolor = para["CHARTCOLOR"].asString();
				streamListe.AddChangeStream(ldid, ldchannel, ldname, intensity,
						chartcolor);
		} else {
				// change led string
				cout << "CHANGE STREAM:" << para.toStyledString() << endl;
				ldid = para["LDID"].asString();

				string ldname = para["LDNAME"].asString();
				string ldchannel = para["LDI2CCHANNEL"].asString();
				string intensity = para["LDLEDARRAY"].toStyledString();
				string chartcolor = para["CHARTCOLOR"].asString();
				int fooddelay = para["fooddelay"].asInt();
				int foodpwr = para["foodpower"].asInt();

				streamListe.SetFoodDelay(fooddelay);
				streamListe.SetFoodPower(foodpwr);

				cout << "ldid: " << ldid << endl;
				cout << "ldchannel: " << ldchannel << endl;
				cout << "ldname: " << ldname << endl;
				streamListe.AddChangeStream(ldid, ldchannel, ldname, intensity,chartcolor);
				//cout << "added change" << endl;
			}

  	}

  	//string output;
  	// creates at the output var a JSON String Object
  	//if (CJsonSerializer::SerializeAjax(&ledListe, output)) {
  	//	cout << "SAVE OKAY" << endl;
  		//MLOG::log("SAVELEDROW: COMMAND ["+command+"] activated  ...",__LINE__,__FILE__);
  		// Sending Back Result for the incoming msg
  		//if (ipcs.sendSockServer("{COMMAND=SAVEROWACKOK}"))
  		//  			MLOG::log("SAVELEDROW: Send ACK OK msg to client..",__LINE__,__FILE__);
  	//}
    }
  /*
  virtual void SetLogger(void (*)logger ){
	  Logger = logger;
  }
  */

  void ConDebugConnectorList(){
	  this->DebugConnectorList();
  }

  Json::Value ConGetConnectorList(){
	  Json::Value streamPowerConnectors = Json::nullValue;

	  streamPowerConnectors["PLUGIN_NAME"]	= pluginname;
	  streamPowerConnectors["PLUGIN_VERSION"]	= pluginversion;
	  streamPowerConnectors["TYPE_OF_PLUGIN"]	= TYPE_OF_PLUGIN;

	  streamPowerConnectors["CONNECTORS"]		= this->ConGetConnectors();

	  cout << "------ STREAM-plugin ConGetConnectorList ----------------" << endl;
	  cout <<  streamPowerConnectors.toStyledString() << endl;
	  cout << "-------------------------------------------------------------" << endl;
	  return streamPowerConnectors;
  }

};


// the class factories
extern "C" Plugin* create() {
	Stream *p = new Stream();
	p->SetFoodTime(0);
	p->SetStopMode(false);
    return p;
}

extern "C" void destroy(Plugin* p) {
	p->~Plugin();
    delete p;
}
#endif



