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



using namespace std;
#include <string>
#include <time.h>
#include "LedString.h"
#include "LedTimerListe.h"
#include "Plugin.hpp"
#include "CJsonSerializer.h"
#include "plugin-config.h"




class Osmosis : public Plugin{
protected:
    LedTimerListe ledListe;				// List of all LED Strings

public:
    virtual string getName(){
    	string name = pluginname;
    	return name ;
    }

    virtual string getVersion(){
    	string version = pluginversion;
    	return version;
    }

    virtual string getTypeOfPlugin(){
        	string typeofplugin = TYPE_OF_PLUGIN;
        	return typeofplugin;
    }

    // Returns A JSON string with all compatible
    // Hardware Adapters that can be used with
    // that Logic plugin
    virtual Json::Value hardwareToUse(){
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
   virtual void Serialize( Json::Value& root )
    {
	   //SetConfigAsJSON( root);
	   ledListe.Serialize(root);
    }


    virtual void Deserialize( Json::Value& root){
    	ledListe.Deserialize(root);
    }

    virtual void SerializeAjax( Json::Value& root ){
    	ledListe.SerializeAjax(root);
    }

    string getConfigHTML(string logic){
    	  return "";
    }

    virtual void work(tm *calltime, Plugin *adapter,Json::Value &json){
    	//adapter->work(NULL,NULL,json);
  }

    void Command( Json::Value& root ){

    }



  virtual void GetConfigAsJSON( string& configval){
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
  virtual void SetConfigAsJSON( Json::Value& para){

   }
  /*
  virtual void SetLogger(void (*)logger ){
	  Logger = logger;
  }
  */

};



// the class factories
extern "C" Plugin* create() {
    return new Osmosis ();
}

extern "C" void destroy(Plugin* p) {
    delete p;
}
#endif



