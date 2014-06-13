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

#ifndef CONFIGREGISTER_H_
#define CONFIGREGISTER_H_
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <dlfcn.h>
#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <cstring>   // Required by strcpy()
#include <cstdlib>   // Required by malloc()
#include <vector>

#include "../json/json.h"
#include "../globals/IJsonSerializable.h"
#include "../marelab/mlog.h"



using namespace std;

class ConfigRegister {
protected:
	string CONFIGFILE ;
	vector<IJsonSerializable*> configRegistry;
	string styled_output;
public:

	void addObj(IJsonSerializable* configobj) {
		configRegistry.push_back(configobj);
	}
	;

	/*
	 * Write all OBJ that where added to the registry
	 * to the config file
	 */
    void writeConfig(){
    	string output;
    	string temp;
    	IJsonSerializable* configObj;
    	std::ofstream out(CONFIGFILE.c_str());
    	Json::Value serializeRoot;
    	MLOG::log("TRY TO WRITE CONFIG SIZE:" + configRegistry.size(),__LINE__,__FILE__);
    	for (unsigned int i=0; i<configRegistry.size(); i++) {
    		configObj =  configRegistry[i];
    		configObj->Serialize(serializeRoot);
    	}

    	Json::StyledWriter writer;
    	output = writer.write(serializeRoot);
    	out << output;
    	out.close();
    	MLOG::log("CONFIG WRITTEN..." ,__LINE__,__FILE__);
    }
    ;

    /*
     *  reads config file and parse it to the OBJ that
     *  are registered at the registry obj
     */
    void readConfig(){
    	Json::Value rootin;
    	Json::Reader reader;
    	IJsonSerializable* configObj;
    	ifstream in;
		in.open(CONFIGFILE.c_str(), std::ifstream::in);
		if (in.fail())
		{
			cout << "Error open marelab.conf file trying local dev alternative ...";
			CONFIGFILE = "marelab-conf/marelab.conf";
			in.open(CONFIGFILE.c_str(), std::ifstream::in);
		}
		if (in.fail())
			exit(-1);
		//ifstream in(CONFIGFILE);
    	std::string input,line;

    	// Read File
    	while(getline(in, line))
    	    	  input += line + "\n";

    	// Deserialize File to Obj
    	for (unsigned int i=0; i<configRegistry.size(); i++) {
    		configObj =  configRegistry[i];

    	    if (ConfigRegister::Deserialize(configObj,input)){
    	    	string file = __FILE__;
    	    	MLOG::log("Config: parse successfull",__LINE__,file);
    	    }
    	    else{
    	    	// report to the user the failure and their locations in the document.
    	    	string conffile = CONFIGFILE;
    	    	string readererr= reader.getFormatedErrorMessages();
    	    	MLOG::log("ERROR: Failed to parse configuration: " + conffile +" "+ readererr,__LINE__,__FILE__);
    	    }
    	}
    	in.close();
    }
    ;

    void readConfigPart(IJsonSerializable* configObj){
		Json::Value rootin;
		Json::Reader reader;
		ifstream in;
		in.open(CONFIGFILE.c_str(), std::ifstream::in);
		if (in.fail()) {
			cout << "Error open marelab.conf file trying local dev alternative ...";
			CONFIGFILE = "marelab-conf/marelab.conf";
			in.open(CONFIGFILE.c_str(), std::ifstream::in);
		}
		if (in.fail())
			exit(-1);

        	std::string input,line;

        	// Read File
        	while(getline(in, line))
        	    	  input += line + "\n";


        	if (ConfigRegister::Deserialize(configObj,input)){
        	    MLOG::log("Config: parse successfull",__LINE__,__FILE__);
        	}
        	else{
        		// report to the user the failure and their locations in the document.
        		string conffile = CONFIGFILE;
        	    string readererr= reader.getFormatedErrorMessages();
        	    MLOG::log("ERROR: Failed to parse configuration: " + conffile +" "+ readererr,__LINE__,__FILE__);
        	}
        	in.close();
       }
        ;

    static bool Serialize(IJsonSerializable* pObj, std::string& output,Json::Value serializeRoot) {
    		if (pObj == NULL)
    			return false;

    		//Json::Value serializeRoot;
    		pObj->Serialize(serializeRoot);
    		Json::StyledWriter writer;
    		output = writer.write(serializeRoot);
    		return true;
    }
    ;

    ~ConfigRegister() {
	}
	;



	static bool SerializeAjax(IJsonSerializable* pObj, std::string& output) {
		if (pObj == NULL)
			return false;

		Json::Value serializeRoot;
		pObj->SerializeAjax(serializeRoot);

		Json::FastWriter writer;
		//Json::StyledWriter writer;
		output = writer.write(serializeRoot);

		return true;
	}
	;

	static bool Deserialize(IJsonSerializable* pObj, std::string& input) {
		if (pObj == NULL)
			return false;

		Json::Value  deserializeRoot;
		Json::Reader reader;

		if (!reader.parse(input, deserializeRoot))
			return false;

		pObj->Deserialize(deserializeRoot);

		return true;
	}
	;
	ConfigRegister(void) {
		CONFIGFILE = "../marelab-conf/marelab.conf";
	}
	;

	//Config getConfigObj(string name){
	//	return 	(Config) NULL;
	//};

};
#endif /* CONFIGREGISTER_H_ */
