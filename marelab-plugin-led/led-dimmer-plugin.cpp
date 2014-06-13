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
#include "LedString.h"
#include "LedTimerListe.h"
#include "Plugin.hpp"
#include "CJsonSerializer.h"
#include "plugin-config.h"
#include "LConnection.h"
#include "json/json.h"

class LedDimmer : public Plugin, LConnection {
protected:
    LedTimerListe ledListe;				// List of all LED Strings
    bool	LIGHT_AUTO_MODE;			// Gets set by the WebInterface for Auto Mode Brightness by Graph
    bool    LIGHT_FULL_BRIGHT;			// Gets set by the WebInterface for FULL bright lightness
    int 	LIGHT_FULL_BRIGHT_VALUE;



public:
    LedDimmer(){
    	LIGHT_AUTO_MODE = true;
    	LIGHT_FULL_BRIGHT = false;
    	LIGHT_FULL_BRIGHT_VALUE = 100;
    }

    ~LedDimmer(){
    	cout << "LedDimmer Destructor..." << endl;
    	ledListe.Clear();
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
	   ledListe.Serialize(root);

	   // Adding the Logical Connectors to the config file
	   Json::Value obj_value(Json::objectValue);
	   for (unsigned int i =0;i < connectorList.size();i++){
			obj_value["Connector"]["description"] 	= connectorList[i]->description;
			obj_value["Connector"]["uuid"] 			= connectorList[i]->UUID;
			obj_value["Connector"]["dir"]			= connectorList[i]->dir;
			obj_value["Connector"]["hdtype"]		= connectorList[i]->hdType;
			root["Connectors"].append(obj_value);
			obj_value.clear();
	   }
    }


    void Deserialize( Json::Value& root){
    	ledListe.Deserialize(root);
    	/*
    	 * Now we create the connetion List that is needed
    	 * to connect locical IO to phy. IO
    	 */
    	Json::Value config;
		// Delete of the list
		this->Clear();
		unsigned int a = 0;
		config = root["LedDimmer"][a];
		unsigned int connectorCount = config.get("Connectors", "").size();
		//MLOG::log("PLUGIN LED: Count of Dim Channels = "+ledchannelcout,__LINE__,__FILE__);
		cout << "LedDimmer Count of Connectors: " << connectorCount << endl;
		Connector *connector;
		for (unsigned int i = 0; i < connectorCount; i++) {

			config.get("Connectors", "")[i].get("Connector", "").get("description", "").asString();
			config.get("Connectors", "")[i].get("Connector", "").get("uuid", "").asString();
			config.get("Connectors", "")[i].get("Connector", "").get("dir", "").asInt();
			config.get("Connectors", "")[i].get("Connector", "").get("hdtype", "").asInt();
			// Adding Connector to the ConnectorList
			connector = new Connector(	static_cast<CONNECTION_TYP>(config.get("Connectors", "")[i].get("Connector", "").get("hdtype", "").asInt()),
										config.get("Connectors", "")[i].get("Connector", "").get("description", "").asString(),
										static_cast<CONNECTION_DIR>(config.get("Connectors", "")[i].get("Connector", "").get("dir", "").asInt()),
										config.get("Connectors", "")[i].get("Connector", "").get("uuid", "").asString());
			this->Add(connector);
		}




    	/*


    	Connector *UP_CTRL = new Connector(IO,"IO Pin for Motor Up",OUT,"e4f0deab-bb25-4243-92d2-eac2c1c7aa59");
    	Connector *DOWN_CTRL = new Connector(IO,"IO Pin for Motor DOWN",OUT,"b54621d4-77d0-4d1c-a4b3-252a86448ecf");
    	this->Add(UP_CTRL);
    	this->Add(DOWN_CTRL);
    	Connector *led_leiste;
    	for (int counter=1; counter <=  ledListe.CountLed(); counter++){
    		LedString* ledstring = ledListe.GetLed(counter);
    		string name = "DA Channel for Dimmer-" + ledstring->getLdName();
    	    led_leiste = new Connector(DA,name,OUT,ledstring->getUUID());
    	    this->Add(led_leiste);
    	}
    	*/
    	cout << "---- LISTE LEDDIMMER CONNETRORS -----" << endl;
    	this->DebugConnectorList();
    	cout << "---- LISTE LEDDIMMER CONNETRORS -----" << endl;
    }

    void SerializeAjax( Json::Value& root ){
    	ledListe.SerializeAjax(root);
    }

    string getConfigHTML(string logic){
    	  return "";
    }

    void work(tm *calltime, Plugin *adapter,Json::Value &LedDimmer){
    	int led_intensity = 0;
	  	time_t 	tim;				// SystemTime
    	tim=time(NULL);
    	//cout << "SIZE of StringList: " << ledListe.CountLed()<< endl;

    	// Finding the configured Hardware Adapter

    	//cout << "CALLED WORK LOGIC : " << this->getName()<< endl;
    	for (int counter=0; counter <  ledListe.CountLed(); counter++){
    		// Calculate the  LED 0-100% Value for a given led string from the graph
    		tim=time(NULL);
    		tm *timeNow = localtime(&tim);

    		if (!LIGHT_AUTO_MODE && LIGHT_FULL_BRIGHT)
    		{
    			ledListe.SetLastPowerValue(counter,timeNow,LIGHT_FULL_BRIGHT_VALUE);
    		    led_intensity = LIGHT_FULL_BRIGHT_VALUE;

    		}

    		if (LIGHT_AUTO_MODE && !LIGHT_FULL_BRIGHT){
    			led_intensity = ledListe.GetPowerValue(counter,timeNow);
    		}



    		if (led_intensity != -1){
    			// Send the value to the register of the configured Hardware Adapter
    			// This is just a value transport the hardware adaption is done in the
    			// Hardware Adapter!!!
    			Json::Value jsonLedDimmer;
    			jsonLedDimmer["LOGIC_PLUGIN_NAME"] = this->getName();
    			jsonLedDimmer["DIMMER_STRING_NO"] = counter;
    			jsonLedDimmer["DIMMER_CHANNEL"] = ledListe.GetLedArrayNo(counter)->getLdI2cChannel();
    			jsonLedDimmer["DIMMER_VALUE"] = led_intensity;
    			adapter->work(calltime,this,jsonLedDimmer);
    		}
    	}



    	if (led_intensity != -1)
    	cout << "--------------------" << endl;
  }

  void Command( Plugin *adapter,Json::Value& root ){
	  string parameter = root.get("LEDIMMER_CONTROL","").asString();
	  if (parameter.compare("DOWN") == 0) {
		  cout <<"COMMAND PARAMETER LEDDIMMER "<< root.toStyledString() << endl;
		  adapter->Command(adapter,root);
	  }
	  else if (parameter.compare("UP") == 0) {
		  cout <<"COMMAND PARAMETER LEDDIMMER "<< root.toStyledString() << endl;
		  adapter->Command(adapter,root);
	  }
	  else if (parameter.compare("STOP") == 0) {
		  cout <<"COMMAND PARAMETER LEDDIMMER "<< root.toStyledString() << endl;
		  adapter->Command(adapter,root);
	  }
	  else if (parameter.compare("AUTO_LIGHT") == 0) {
		  cout <<"LIGHT SET TO AUTO MODE..."<< endl;
		  LIGHT_AUTO_MODE   = true;
		  LIGHT_FULL_BRIGHT = false;
	  }
	  else if (parameter.compare("FULL_LIGHT") == 0) {
		  cout <<"LIGHT SET TO FULL BRIGHT..."<< endl;
		  LIGHT_AUTO_MODE   = false;
		  LIGHT_FULL_BRIGHT = true;
	  }
  }


  // NUCLEUS WANTS TO GET THE CONFIG FOR THIS PLUGIN
  void GetConfigAsJSON( string& configval){
	  CJsonSerializer::SerializeAjax(&ledListe, configval);
  }

  /*
   * Getting the parameter for the LED that
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
  			ledListe.DelLed(atoi(id2del.c_str()));

  		}
  		else if (para["oper"].asString().compare("add") == 0) {
  			cout << "ADD LED:" << para.toStyledString() << endl;
  			ldid = para["id"].asString();
  			string ldname = para["LDNAME"].asString();
  			string ldchannel = para["LDI2CCHANNEL"].asString();
  			string intensity = para["LDLEDARRAY"].toStyledString();
  			string chartcolor = para["CHARTCOLOR"].asString();
  			string uuid =  para["UUID"].asString();
  			ledListe.AddChangeLed(ldid, ldchannel, ldname, intensity,chartcolor,uuid);
  		}
  		else if (para["oper"].asString().compare("edit") == 0) {
				cout << "EDIT LED:" << para.toStyledString() << endl;
				ldid = para["id"].asString();
				string ldname = para["LDNAME"].asString();
				string ldchannel = para["LDI2CCHANNEL"].asString();
				string intensity = para["LDLEDARRAY"].toStyledString();
				string chartcolor = para["CHARTCOLOR"].asString();
				string uuid =  para["UUID"].asString();
				ledListe.AddChangeLed(ldid, ldchannel, ldname, intensity,chartcolor,uuid);
		} else {
				// change led string
				cout << "CHANGE LED:" << para.toStyledString() << endl;
				ldid = para["LDID"].asString();

				string ldname = para["LDNAME"].asString();
				string ldchannel = para["LDI2CCHANNEL"].asString();
				string intensity = para["LDLEDARRAY"].toStyledString();
				string chartcolor = para["CHARTCOLOR"].asString();
				cout << "ldid: " << ldid << endl;
				cout << "ldchannel: " << ldchannel << endl;
				cout << "ldname: " << ldname << endl;
				string uuid =  para["UUID"].asString();
				ledListe.AddChangeLed(ldid, ldchannel, ldname, intensity,chartcolor,uuid);
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
	  Json::Value ledDimmerConnectors = Json::nullValue;

	  ledDimmerConnectors["PLUGIN_NAME"]	= pluginname;
	  ledDimmerConnectors["PLUGIN_VERSION"]	= pluginversion;
	  ledDimmerConnectors["TYPE_OF_PLUGIN"]	= TYPE_OF_PLUGIN;

	  ledDimmerConnectors["CONNECTORS"]		= this->ConGetConnectors();
	  cout << "------ LED-DIMMER-plugin ConGetConnectorList ----------------" << endl;
	  cout <<  ledDimmerConnectors.toStyledString() << endl;
	  cout << "-------------------------------------------------------------" << endl;
	  return ledDimmerConnectors;
  }

};


// the class factories
extern "C" Plugin* create() {
    return new LedDimmer;
}

extern "C" void destroy(Plugin* p) {
	p->~Plugin();
    delete p;
}
#endif



