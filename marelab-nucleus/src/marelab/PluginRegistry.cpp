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

#include "PluginRegistry.h"
#include <vector>
#include <string>
#include <iostream>
#include <dlfcn.h>
#include <cstring>
#include "ConfigNucleus.h"
#include "ConfigRegister.h"
#include "mlog.h"



using namespace std;



PluginRegistry::PluginRegistry(ConfigNucleus *configMarelab) {
	this->configMarelab = configMarelab;
}


bool PluginRegistry::isFilePlugin(string filename)
{

	MLOG::log("TRY TO LOAD PLUGIN: "+filename,__LINE__,__FILE__ );
	//void* pluginHandle = dlopen(filename.c_str(), RTLD_LAZY);
    void* pluginHandle = dlopen(filename.c_str(), RTLD_NOW|RTLD_GLOBAL);
    if (!pluginHandle) {
       MLOG::log("PLUGIN Loader: File ("+filename+ ") is no Plugin :"+dlerror(),__LINE__,__FILE__ );
      // cout << "PLUGIN Loader: File ("<< filename << ") is no Plugin :" << dlerror() << '\n';
       return false;
    }

    // load the symbols
    //create_t* create_plugin = (create_t*)
    dlsym(pluginHandle, "create");
    const char* dlsym_error = dlerror();
    if (dlsym_error) {
    	string err = dlsym_error;
    	MLOG::log("PLUGIN Loader: Cannot load symbol create: " + err,__LINE__,__FILE__ );
    	//cout << "PLUGIN Loader: Cannot load symbol create: " << dlsym_error << '\n';
        return 1;
    }

   // destroy_t* destroy_plugin = (destroy_t*)
   dlsym(pluginHandle, "destroy");
   dlsym_error = dlerror();
   if (dlsym_error) {
	   string err = dlsym_error;
	   MLOG::log("PLUGIN Loader: Cannot load symbol destroy: "+err,__LINE__,__FILE__ );
	   return 1;
   }


   return true;
}





/*
 * Clears the pluginList of the registry
 */
void PluginRegistry::ClearRegistry(){
	// Lets remove all entrys in the list
	// Destroy the object & clear the handle
	PluginObject *pluginobj;
	for (unsigned int i=0; i<pluginList.size(); i++) {
	    pluginobj =  pluginList[i];
	    pluginobj->plugin_destroyFunc(pluginobj->plugin);
	    dlclose(pluginobj->pluginHandle);
	    delete pluginobj;

	}
	pluginList.clear();
	//delete pluginList;

	for (unsigned int i=0; i<adapterList.size(); i++) {
		pluginobj =  adapterList[i];
		pluginobj->plugin_destroyFunc(pluginobj->plugin);
		dlclose(pluginobj->pluginHandle);
		delete pluginobj;

	}
	adapterList.clear();
	//delete adapterList;
}

/*
void PluginRegistry::PluginsAddToConfig(ConfigRegister *configRegistry){
	PluginObject *pluginobj;
	for (int i=0; i<pluginList.size(); i++) {
		    pluginobj =  pluginList[i];
		    //configRegistry->addObj(pluginobj->plugin);
		    configRegistry->addObj(pluginobj);
		}
}
*/

int PluginRegistry::loadPluging(string filename){
		string typeofplug;
		void* pluginhandle = dlopen(filename.c_str(), RTLD_LAZY);
	    if (!pluginhandle) {
	       return false;
	    }

	    // load the symbols
	    create_t* create_plugin = (create_t*) dlsym(pluginhandle, "create");
	    const char* dlsym_error = dlerror();
	    if (dlsym_error) {
	    	string err = dlsym_error;
	    	MLOG::log("PLUGIN Loader: Cannot load symbol create: "+err,__LINE__,__FILE__ );
	    	return 1;
	    }

	   destroy_t* destroy_plugin = (destroy_t*) dlsym(pluginhandle, "destroy");
	   dlsym_error = dlerror();
	   if (dlsym_error) {
		   string err = dlsym_error;
		   MLOG::log("PLUGIN Loader: Cannot load symbol destroy: "+err,__LINE__,__FILE__ );
		   return 1;
	   }


	   // create an instance of the class
	   PluginObject *pluginobj = new PluginObject;
	   pluginobj->plugin_createFunc = create_plugin;
	   pluginobj->plugin_destroyFunc= destroy_plugin;
	   pluginobj->pluginHandle=pluginhandle;
	   pluginobj->plugin = create_plugin();
	   pluginobj->SetTypeOfPlugin(pluginobj->plugin->getTypeOfPlugin());

	   // use the class
	   //pluginobj->plugin->work(NULL,NULL);
	   string name = pluginobj->plugin->getName();
	   MLOG::log("Plugin: loaded pugin ["+name +"]",__LINE__,__FILE__ );
	   //pluginobj->plugin->SetLogger(&MLOG);
	   typeofplug = pluginobj->plugin->getTypeOfPlugin();
	   // Adding to plugin list
	   if (strcmp(typeofplug.c_str(),"LOGIC")==0){
		   MLOG::log("LOGIC PLUGIN ADDED :" +pluginobj->plugin->getName(),__LINE__,__FILE__ );
		   pluginList.push_back(pluginobj);
	   }
	   // Adding to adapterlist
	   if (strcmp(typeofplug.c_str(),"ADAPTER")==0){
		   MLOG::log("Adapter PLUGIN ADDED :" +pluginobj->plugin->getName(),__LINE__,__FILE__ );
		   adapterList.push_back(pluginobj);
	   }
	   return true;
}

/*
 * Scans the plugin dir for all plugins and generates a list
 * for loading the plugins
 */
int PluginRegistry::ScanForPlugins()
{
	string dir = configMarelab->getCfPlugindir();

	DIR *dp;
	struct dirent *dirp;

	if((dp  = opendir(dir.c_str())) == NULL) {
		 stringstream ss;//create a stringstream
		   ss << errno;//add number to the stream
		  string err =  ss.str();
		MLOG::log("PLUGIN Loader: Error opening PluginDir (" +err+") " +dir,__LINE__,__FILE__ );
		return errno;
	 }

	while ((dirp = readdir(dp)) != NULL) {
		string filename = dirp->d_name;
		if (isFilePlugin (dir+"/"+filename))
		{

			loadPluging(dir+"/"+filename);
		}
	}
	closedir(dp);

	// Now all plugin are loaded plugin Objects are created
	// We have to set the references between plugin Objects
	// logicPlugin -> adapterPlugin
	/*PluginObject *pluginobj;
	for (unsigned int i=0; i < pluginList.size(); i++) {
			pluginobj =  pluginList[i];
			pluginobj->adapter = getAdapterObject(pluginobj->)
			if (pluginobj->plugin->getName()== name)
				return pluginobj;
	}*/
	return 0;
}

/*
Plugin* getAdapterObject(string adapterName){
	PluginObject *pluginobj;
	for (unsigned int i=0; i < adapterList.size(); i++) {
				pluginobj =  adapterList[i];
				if (pluginobj->plugin->getName()== name)
					return pluginobj;
		}

	return NULL;
}
*/

PluginObject* PluginRegistry::GetPluginWithName(string name){
	PluginObject *pluginobj;
	for (unsigned int i=0; i < pluginList.size(); i++) {
		pluginobj =  pluginList[i];
		if (pluginobj->plugin->getName()== name){
			return pluginobj;
		}
	}
	return NULL;
}

PluginObject* PluginRegistry::GetAdapterWithName(string name){
	PluginObject *pluginobj;
	for (unsigned int i=0; i < adapterList.size(); i++) {
		pluginobj =  adapterList[i];

		if (pluginobj->plugin->getName()== name){
			return pluginobj;
		}

	}
	return NULL;
}

/*
 * Itterates throu the registry and gives back a JSON string with the
 * names of the founded plugins.
 */
string PluginRegistry::JSONgetPluginFileNames(){
	PluginObject *pluginobj;
	string jsonlist;
	string pathtoplugs = configMarelab->getCfPlugindir();
	string typeplugin;
	Json::Value pluginlist;
	Json::Value plugin2add;

	for (unsigned int i=0; i < pluginList.size(); i++) {
		pluginobj =  pluginList[i];
		plugin2add["PLUGINNAME"] = pluginobj->plugin->getName();
		pluginlist.append(plugin2add);
	}
	jsonlist = pluginlist.toStyledString();
	return jsonlist;
}

/*
 * SAVE
 */
void PluginRegistry::Serialize( Json::Value& root ){
	PluginObject *pluginobj;
	Json::Value pluginItem;

	// SAVE ALL LOGIC CONFIGS
	for (unsigned int i=0; i < pluginList.size(); i++) {
		pluginobj =  pluginList[i];
		pluginItem.clear();
		if (pluginobj->plugin->getTypeOfPlugin()== "LOGIC"){
			// Saving the binding of a LOGIC-TO APADTER PLUG
			pluginobj->SaveConfig(pluginItem);
			root["LOGIC_ADAPTER_PLUGINS_BINDING"].append(pluginItem);
			pluginItem.clear();
			// Saving Plugin delivered var objects
			pluginobj->Serialize(pluginItem);
			MLOG::log("PLUGIN-LOGIC SAVED: " +pluginobj->plugin->getName(),__LINE__,__FILE__ );
			root[pluginobj->plugin->getName()].append(pluginItem);
		}
	}

	// SAVE ALL ADAPTER CONFIGS
	for (unsigned int i=0; i < adapterList.size(); i++) {
		pluginobj =  adapterList[i];
		pluginItem.clear();
		if (pluginobj->plugin->getTypeOfPlugin()== "ADAPTER"){
				MLOG::log("PLUGIN-ADAPTER SAVED: " + pluginobj->plugin->getName() ,__LINE__,__FILE__ );
				pluginobj->Serialize(pluginItem);
				root[pluginobj->plugin->getName()]  = pluginItem;
		}
	}
}

/*
 * RESTORE
 */
void PluginRegistry::Deserialize( Json::Value& root){
	PluginObject* LogicPlugin = NULL;
	PluginObject* AdapterPlugin = NULL;
	// Restore the Logic->Adapter Plugin bindings from config file
	for (unsigned int i=0; i < root["LOGIC_ADAPTER_PLUGINS_BINDING"].size(); i++) {
		MLOG::log("Deserialize: " + root["LOGIC_ADAPTER_PLUGINS_BINDING"][i]["AdapterPluginName"].asString(),__LINE__,__FILE__ );
		LogicPlugin   = GetPluginWithName(root["LOGIC_ADAPTER_PLUGINS_BINDING"][i]["LogicPluginName"].asString());
		AdapterPlugin = GetAdapterWithName(root["LOGIC_ADAPTER_PLUGINS_BINDING"][i]["AdapterPluginName"].asString());
		if (AdapterPlugin != NULL){
			MLOG::log("Deserialize Adapter: " +  AdapterPlugin->plugin->getName(),__LINE__,__FILE__ );
			AdapterPlugin->plugin->Deserialize(root);
		}
		else
		{
			MLOG::log("ERROR no adapter plugin  is there !!!!",__LINE__,__FILE__ );
			exit (-1);
		}
		LogicPlugin->adapter = AdapterPlugin->plugin;
	}
	// Restore Plugin Parameters from Config
	PluginObject *pluginobj;
	for (unsigned int i=0; i < pluginList.size(); i++) {
		pluginobj =  pluginList[i];
		MLOG::log("PluginRegistry pluginList Deserilize:" + pluginobj->plugin->getName(),__LINE__,__FILE__ );
		pluginobj->Deserialize( root );
	}
}

void PluginRegistry::SerializeAjax( Json::Value& root ){
	PluginObject *pluginobj;

	Json::Value obj_value(Json::objectValue);
	obj_value["COMMAND"] = "GET_PLUGININFO";

	//obj_value["PARAMETER"] = "GET_PLUGININFO";
	root["COMMAND"].append(obj_value);
	string typeplugin;
	//{"COMMAND":"GET_PLUGININFO","PARAMETER":"
	for (unsigned int i=0; i < pluginList.size(); i++) {
			pluginobj =  pluginList[i];
			typeplugin = pluginobj->plugin->getTypeOfPlugin();
			// Only Logic Plugins get listed
			if (strcmp(typeplugin.c_str(),"LOGIC")==0){
				MLOG::log(pluginobj->plugin->getName(),__LINE__,__FILE__ );
				obj_value["PLUGINNAME"]    = pluginobj->plugin->getName();
				obj_value["PLUGINVERSION"] = pluginobj->plugin->getVersion();
				MLOG::log(pluginobj->plugin->getVersion(),__LINE__,__FILE__ );
				//pluginobj->plugin->hardwareToUse();
				obj_value["PLUGINHARDWARE"]  = pluginobj->plugin->hardwareToUse();
				obj_value["SELECTEDADAPTER"] = pluginobj->adapter->getName();
				root["PARAMETER"].append(obj_value);
				obj_value.clear();
			}
	}
}

void PluginRegistry::SerializeAdapter( Json::Value& root ){
	PluginObject *pluginobj;

	Json::Value obj_value(Json::objectValue);
	obj_value["COMMAND"] = "GET_ADAPTERINFO";

	//obj_value["PARAMETER"] = "GET_PLUGININFO";
	root["COMMAND"].append(obj_value);
	string typeplugin;
	//{"COMMAND":"GET_PLUGININFO","PARAMETER":"
	for (unsigned int i=0; i < adapterList.size(); i++) {
			pluginobj =  adapterList[i];
			typeplugin = pluginobj->plugin->getTypeOfPlugin();
			// Only Logic Plugins get listed
			if (strcmp(typeplugin.c_str(),"ADAPTER")==0){
				obj_value["PLUGINNAME"] = pluginobj->plugin->getName();
				obj_value["PLUGINVERSION"] = pluginobj->plugin->getVersion();
				root["PARAMETER"].append(obj_value);
				obj_value.clear();
			}
	}
}
