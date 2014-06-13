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

#ifndef PLUGINREGISTRY_H_
#define PLUGINREGISTRY_H_

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>
#include <dlfcn.h>
#include "ConfigRegister.h"
#include "../globals/Plugin.hpp"
#include "ConfigNucleus.h"

using namespace std;

class PluginObject : public IJsonSerializable{

private:
	string typeOfPlugin;  		// Signals Hardware or function Plugin a function Plugin gets linked to a hardware Plugin

public:
	void* pluginHandle;
	create_t* plugin_createFunc;
	destroy_t* plugin_destroyFunc;
	Plugin* plugin;
	Plugin* adapter;			// A logic Plugin has to get a Adapter Plugin assigned to work

    //IO
	// Special Save function to Save all Logic Plugin to Adapter Bindings
    void SaveConfig( Json::Value& root ){
    	root["LogicPluginName"] 			= plugin->getName();
    	root["LogicPluginNameVersion"]		= plugin->getVersion();
    	root["AdapterPluginName"]			= adapter->getName();
    	root["AdapterPluginNameVersion"]	= adapter->getVersion();
    };
    void ReadConfig( Json::Value& root){

    };

    string GetTypeOfPlugin(){
    	return typeOfPlugin;
    }

    void SetTypeOfPlugin(string type){
       	typeOfPlugin = type;
    }

    virtual void Serialize( Json::Value& root ){
    	plugin->Serialize(root);
    };
    virtual void Deserialize( Json::Value& root){
    	plugin->Deserialize(root);
    };
    virtual void SerializeAjax( Json::Value& root ){};
    virtual void GetConfigAsJSON( string& configval){};
    virtual void SetConfigAsJSON( Json::Value& para){};
};



class PluginRegistry : public IJsonSerializable{
private:
	vector<PluginObject*> 	pluginList;			// List of LOGIC plugs
	vector<PluginObject*> 	adapterList;		// List of Adapter Hardware plugs

	ConfigNucleus* configMarelab;


public:
	PluginRegistry(ConfigNucleus *configMarelab);
	virtual ~PluginRegistry(){
		cout << "Destructor PluginRegistriy..." << endl;
		ClearRegistry();
	}
	void ClearRegistry();						// Removes Logic & Adapter Plugs
	int ScanForPlugins(); 						// scans for Plugins
	bool isFilePlugin(string filename); 		// checks file is it a plugin
	int loadPluging(string filename);			// loads a Plugin
	int getPluginCount(){ return pluginList.size();}
	int getAdapterCount(){ return adapterList.size();}
	Plugin* getPluginObject(int number){ return pluginList[number]->plugin;}
	Plugin* getAdapterObject(int number){ return adapterList[number]->adapter;}
	Plugin* getAdapterObject(string adapterName);
	void PluginsAddToConfig(ConfigRegister *configRegistry);
	PluginObject* GetAdapterEntry(int id){ return adapterList[id];}
	PluginObject* GetPluginEntry(int id){ return pluginList[id];}
	PluginObject* GetPluginWithName(string name);
	PluginObject* GetAdapterWithName(string name);
	string JSONgetPluginFileNames();
	void SerializeAdapter( Json::Value& root );

	string DebugAdapterList(){
		string debugstring ="";
		PluginObject *pluginobj;

		for (unsigned int i=0; i < adapterList.size(); i++) {
				pluginobj =  adapterList[i];
				if (pluginobj->plugin->getTypeOfPlugin()== "ADAPTER"){
					debugstring = debugstring + "AdapterName:" + pluginobj->plugin->getName() +"\r\n";
					debugstring = debugstring + "Type:" + pluginobj->plugin->getTypeOfPlugin() +"\r\n";
					debugstring = debugstring +"\r\n";
				}
		}
		return debugstring;
	};

	string DebugLogicList(){
			string debugstring ="";
			PluginObject *pluginobj;

			for (unsigned int i=0; i < pluginList.size(); i++) {
					pluginobj =  pluginList[i];
					if (pluginobj->plugin->getTypeOfPlugin()== "LOGIC"){
						debugstring = debugstring + "PluginLogicName:" + pluginobj->plugin->getName() +"\r\n";
						debugstring = debugstring + "Type:" + pluginobj->plugin->getTypeOfPlugin() +"\r\n";
						debugstring = debugstring +"\r\n";
					}
			}
			return debugstring;
		};

	//PERSISTENT INTERFACE
	virtual void Serialize( Json::Value& root );
	virtual void Deserialize( Json::Value& root);
	virtual void SerializeAjax( Json::Value& root );

};



#endif /* PLUGINREGISTRY_H_ */
