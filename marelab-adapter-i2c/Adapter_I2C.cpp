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

#ifndef Adapter_I2C_HPP_
#define Adapter_I2C_HPP_

using namespace std;
#include <string>
#include <time.h>
#include "Plugin.hpp"
#include "plugin-adapterconfig.h"
#include "devices/I2C.h"
#include "devices/PCF8574.h"
#include "devices/MAX520.h"
#include "devices/I2CDEVICE.h"

class Adapter_I2C : public Plugin{

private:
	vector<I2C_DEVICE*> 	i2c_device_list;

public:
	Adapter_I2C(){

		 /*
		  * Define PCF8574
		  * IO ports 8
		  */
	}
	virtual ~Adapter_I2C(){
		//delete pcf8574;
		cout << "Adapter_I2C Destructor..." << endl;
		Clear_I2C_DeviceList();
		i2c_device_list.clear();
	}

	// Clear all Plugins from adapter list
	void Clear_I2C_DeviceList(){
		for (unsigned int i=0; i < i2c_device_list.size(); i++)
			delete i2c_device_list[i];

	}

	/*
	 * Adding a I2C Chip to the list
	 * TODO:
	 * if successfull a 0 is returned
	 * if I2C Adress is taken or more then 127 devices are there a 1 is returned
	 */
	int Add_I2C_Device(I2C_DEVICE* device){
		i2c_device_list.push_back(device);
		return 0;
	}

	void* Get_I2C_Device(int id){
		for (unsigned int i=0; i < i2c_device_list.size(); i++) {
			if (i2c_device_list[i]->getDeviceNo()  == id){
				return i2c_device_list[i]->getDeviceClass();
			}
		}
		return NULL;
	}

	string getName() {
		string name = pluginname;
		return name;
	}

	string getVersion() {
		string version = pluginversion;
		return version;
	}

	string getTypeOfPlugin() {
		string typeofplugin = TYPE_OF_PLUGIN;
		return typeofplugin;
	}


	void work(tm *calltime, Plugin *adapter,Json::Value &json) {
		if (!json.null)
		{
			// Which modul is used by LED
			if ((json["LOGIC_PLUGIN_NAME"]=="LedDimmer") ||(json["LOGIC_PLUGIN_NAME"]=="Stream"))
			{
				cout << "I2CWORK-FROM-ADAPTER PLUGIN:"<< adapter->getName() << " DIMMVALUE:" << json["DIMMER_VALUE"].asInt() << "CH: "<< json["DIMMER_CHANNEL"].asInt() << endl;

				// Here are the details to drive the Hardware
				// TODO: Select ChipTyp according to config
				#ifdef CHIPSET_MAX520
				{
					// TODO: Select Chip according to config
					MAX520 *max520  =(MAX520*) Get_I2C_Device(2);

					max520->setDimChannel(json["DIMMER_CHANNEL"].asInt(),json["DIMMER_VALUE"].asInt());
				}

				#endif
				#ifdef CHIPSET_PCA9685
				{
					vector <Hardware*> hardwareListe;
					for(int i=0; i<logiclistsize; i++){
						logic = logicList[i];
						if (logic->GetName()== "LedDimmer"){
							hardwareListe = logic->getHardwareList();
							for(unsigned int ii=0; ii<hardwareListe.size(); ii++){
								if (hardwareListe[ii]->getName() == "PCA9685"){
									hardwareListe[ii]->DimChannel(json["DIMMER_CHANNEL"].asInt(),json["DIMMER_VALUE"].asInt());
								}
							}
						}
					}
				}
				#endif
			}
		}
	}

	void Command( Plugin *adapter,Json::Value& root ){
		string parameter = root.get("LEDIMMER_DIR","").asString();

		PCF8574 *pcf8574  =(PCF8574*) Get_I2C_Device(1);

		if (parameter.compare("DOWN") == 0) {
			cout <<"I2C PARAMETER LEDDIMMER "<< root.toStyledString() << endl;
			cout << "DIMM ORDER DOWN FOR:" << endl;
			pcf8574->set(0,true);
			pcf8574->set(1,false);
		}
		else if (parameter.compare("UP") == 0) {
			cout <<"I2C  PARAMETER LEDDIMMER "<< root.toStyledString() << endl;
			cout << "DIMM ORDER UP FOR:" << endl;
			pcf8574->set(0, false);
			pcf8574->set(1, true);
		}
		else if (parameter.compare("STOP") == 0) {
			cout <<"I2C  PARAMETER LEDDIMMER "<< root.toStyledString() << endl;
			cout << "DIMM ORDER DOWN FOR:" << endl;
			pcf8574->set(0, false);
			pcf8574->set(1, false);
		}

	}

	string getConfigHTML(string logicname){
		// Getting Chips for configurtaion
		string output="";

		/*
		int logiclistsize = logicList.size();
		string chipName;

		// getting old config

		// preparing html select


		output = output + "<table width='443' border='0' cellspacing='0' cellpadding='0'>";
		output = output + "  <tr>";
		output = output + "    <td><table width='100%' border='0' cellpadding='4' cellspacing='4'>";
		output = output + "      <tr>";
		output = output + "        <td colspan='2' bgcolor='#0099CC'>Add Hardwarechips to BUS</td>";
		output = output + "        <td align='right' bgcolor='#0099CC'>&nbsp;</td>";
		output = output + "      </tr>";
		output = output + "      <tr>";
		output = output + "        <td width='27%' bgcolor='#0099CC'>Cips:";
		output = output + "          <label for='ID_I2CADRESS'></label></td>";
		output = output + "        <td width='53%' align='right' bgcolor='#0099CC'><p>";
		output = output + "          <select name='CHIPSET_SELECT' id='CHIPSET_SELECT'>";
		Logic *logic;
		vector <Hardware*> hardwareListe;
		string HTML_DRIVER_OBJ="";
		for(int i=0; i<logiclistsize; i++){
			logic = logicList[i];
			if (logic->GetName()== logicname){
				hardwareListe = logic->getHardwareList();
				HTML_DRIVER_OBJ="CHIPS={";
				for(unsigned int ii=0; ii<hardwareListe.size(); ii++){
					output = output + "<option value='"+hardwareListe[ii]->getName()+"'>"+hardwareListe[ii]->getName()+"</option>";
					HTML_DRIVER_OBJ=HTML_DRIVER_OBJ+"chipname=&quot;"+hardwareListe[ii]->getName()+"&quot;,HTML=&quot;"+logic->getHardwareLogicByName(hardwareListe[ii]->getName())->getHARDWARE_HTMLConfig()+"&quot;";

				}
			}
		}
		output = output + "          </select>";
		output = output + "        </p></td>";
		output = output + "        <td width='20%' align='right' bgcolor='#0099CC'><input type='image' onclick='HtmlInjection(&quot;HARDWARECHIPS&quot;,&quot;TEST ADD TO TABLE&quot;);' src='img/plus.png' width='31' height='31' alt='ghg' /><img src='img/savef.png' width='32' height='32' alt='ghg' /></td>";
		output = output + "      </tr>";
		output = output + "    </table></td>";
		output = output + "  </tr>";
		output = output + "  <tr><td><span id='HARDWARECHIPS'></span></td></tr>";
		output = output + "</table>";
*/
		return output;
	}

  Json::Value hardwareToUse(){
	  return Json::nullValue ;
  }

  //IO
  void Serialize( Json::Value& root ){
	  std::cout << "WRITE SERILIZE-I2C Adapter" << endl;
	  //root["I2C_BUS"] = i2c.getI2CBus();

	  root["ADR_MAX520_1"]=22;
	  root["ADR_PCF8574_1"]=20;
	  Json::Value i2cvalue;
	  I2C_DEVICE *i2cdev;
	  cout << "**********I2C DEVIVE LIST LEN =" << i2c_device_list.size()<< endl;
	  //cout << root.toStyledString();

	  for (unsigned int i=0; i < i2c_device_list.size(); i++) {
		  i2cdev =  i2c_device_list[i];
		  i2cvalue.clear();
		  i2cvalue["DEVICE_ADR"] = i2cdev->getDeviceAdr();
		  i2cvalue["DEVICE_NAME"] = i2cdev->getDeviceName();
		  i2cvalue["DEVICE_NO"] = i2cdev->getDeviceNo();
		  i2cvalue["DEVICE_BUS"] = i2cdev->getI2cBus();

		  root["CHIPS"].append(i2cvalue);
	  	}
  }

  void Deserialize( Json::Value& root){
	  std::cout << "READ I2C-BUS FROM CONFIG:" << root["I2C Adapter"]["I2C_BUS"].asString() << endl;
	  // i2c bus read
	  //i2c_bus = root["I2C Adapter"]["I2C_BUS"].asString() ;
	  //i2c.setI2CBus(i2c_bus);
	  //i2c.setI2CAdr(1);
	  Clear_I2C_DeviceList();
	  i2c_device_list.clear();
	  cout << "**********I2C DEVIVE LIST LEN =" << i2c_device_list.size()<< endl;
	  std::cout << "READ I2C-DEVICE COUNT before=" << root["I2C Adapter"]["CHIPS"].size() << endl;
	  // read all i2c devices
	  for (unsigned int i=0; i < root["I2C Adapter"]["CHIPS"].size(); i++) {
	  		if (root["I2C Adapter"]["CHIPS"][i]["DEVICE_NAME"].asString()=="PCF8574")
	  		{
	  			cout << "CREATE I2C CHIPS INSTANCE FOR: " << root["I2C Adapter"]["CHIPS"][i]["DEVICE_NAME"].asString()<<endl;
	  			I2C_DEVICE* device;

	  			string device_name 		= root["I2C Adapter"]["CHIPS"][i]["DEVICE_NAME"].asString();
	  			int device_adr 			= root["I2C Adapter"]["CHIPS"][i]["DEVICE_ADR"].asInt();
	  			string device_bus 		= root["I2C Adapter"]["CHIPS"][i]["DEVICE_BUS"].asString();
	  			int device_no 			= root["I2C Adapter"]["CHIPS"][i]["DEVICE_NO"].asInt();
	  			void* i2c_chip_class 	= (void*) new PCF8574(device_bus,device_adr);
	  			device = new I2C_DEVICE(device_name,device_adr,device_bus,device_no,i2c_chip_class);
	  			Add_I2C_Device(device);
	  		}
	  		if (root["I2C Adapter"]["CHIPS"][i]["DEVICE_NAME"].asString()=="MAX520")
	  		{
	  			cout << "CREATE I2C CHIPS INSTANCE FOR: " << root["I2C Adapter"]["CHIPS"][i]["DEVICE_NAME"].asString()<<endl;
	  			I2C_DEVICE* device;

	  			string device_name 		= root["I2C Adapter"]["CHIPS"][i]["DEVICE_NAME"].asString();
	  			int device_adr 			= root["I2C Adapter"]["CHIPS"][i]["DEVICE_ADR"].asInt();
	  			string device_bus 		= root["I2C Adapter"]["CHIPS"][i]["DEVICE_BUS"].asString();
	  			int device_no 			= root["I2C Adapter"]["CHIPS"][i]["DEVICE_NO"].asInt();
	  			void* i2c_chip_class 	= (void*) new MAX520(device_bus,device_adr);
	  			device = new I2C_DEVICE(device_name,device_adr,device_bus,device_no,i2c_chip_class);
	  			Add_I2C_Device(device);
	  		}
	  	}
	  std::cout << "READ I2C-DEVICE COUNT after=" << root["I2C Adapter"]["CHIPS"].size() << endl;
	  cout << "**********I2C DEVIVE LIST LEN =" << i2c_device_list.size()<< endl;
	  /* Test if all i2c chips have a LConnection info */
	  for (unsigned int i=0; i < i2c_device_list.size(); i++) {
		  if (i2c_device_list[i]->getDeviceName() == "MAX520"){
			  MAX520* max 	=  (MAX520*)i2c_device_list[i]->getDeviceClass();
			  max->DebugConnectorList();
		  }
		  else if (i2c_device_list[i]->getDeviceName() == "PCF8574"){
			  PCF8574* pcf 	=  (PCF8574*)i2c_device_list[i]->getDeviceClass();
			  pcf->DebugConnectorList();
		  }
	  }
  }

  void SerializeAjax( Json::Value& root ){}
  void GetConfigAsJSON( string& configval){}
  void SetConfigAsJSON( Json::Value& para) {}

  void ConDebugConnectorList(){
	  for (unsigned int i=0; i < i2c_device_list.size(); i++) {
	 		  if (i2c_device_list[i]->getDeviceName() == "MAX520"){
	 			  MAX520* max 	=  (MAX520*)i2c_device_list[i]->getDeviceClass();
	 			  max->DebugConnectorList();
	 		  }
	 		  else if (i2c_device_list[i]->getDeviceName() == "PCF8574"){
	 			  PCF8574* pcf 	=  (PCF8574*)i2c_device_list[i]->getDeviceClass();
	 			  pcf->DebugConnectorList();
	 		  }
	 	  }
  }

  Json::Value ConGetConnectorList(){
 	  Json::Value adapterConnectors = Json::nullValue;
 	  Json::Value adapterChips 		= Json::nullValue;

 	  adapterConnectors["PLUGIN_NAME"]	   	= pluginname;
 	  adapterConnectors["PLUGIN_VERSION"]	= pluginversion;
 	  adapterConnectors["TYPE_OF_PLUGIN"]	= TYPE_OF_PLUGIN;


 	  for (unsigned int i=0; i < i2c_device_list.size(); i++) {
 		 adapterChips.clear();
 		 //cout << "ADAPTER DEVICE NAME = " << i2c_device_list[i]->getDeviceName() << endl;
 		  if (i2c_device_list[i]->getDeviceName() == "MAX520"){
 			  MAX520* max 					=  	(MAX520*)i2c_device_list[i]->getDeviceClass();
 			  adapterChips["CHIPTYP"]		=  	i2c_device_list[i]->getDeviceName();
 			  adapterChips["DEVICE_ADR"]	=  	i2c_device_list[i]->getDeviceAdr();
 			  adapterChips["DEVICE_NO"]		=  	i2c_device_list[i]->getDeviceNo();
 			  adapterChips["DEVICE_BUS"]	=  	i2c_device_list[i]->getI2cBus();
 			  adapterChips["CONNECTORS"]    = 	max->ConGetConnectors();
 	 	  }
 		  else if (i2c_device_list[i]->getDeviceName() == "PCF8574"){
 			  PCF8574* pcf 				=   (PCF8574*)i2c_device_list[i]->getDeviceClass();
 			  adapterChips["CHIPTYP"]		=  	i2c_device_list[i]->getDeviceName();
 			  adapterChips["DEVICE_ADR"]	=  	i2c_device_list[i]->getDeviceAdr();
 			  adapterChips["DEVICE_NO"]		=  	i2c_device_list[i]->getDeviceNo();
 			  adapterChips["DEVICE_BUS"]	=  	i2c_device_list[i]->getI2cBus();
 			  adapterChips["CONNECTORS"]= 	pcf->ConGetConnectors();
 		//	  cout << adapterChips["CONNECTORS"].toStyledString()<< endl;
 		  }
 		 adapterConnectors["CHIPS_ON_BUS"].append(adapterChips);
 	  }
 	  //cout << "------ ADAPTER-I2C-plugin ConGetConnectorList ---------------" << endl;
 	  //cout <<  adapterConnectors.toStyledString() << endl;
 	  //cout << "-------------------------------------------------------------" << endl;
 	  return adapterConnectors;
   }

};

// the class factories
extern "C" Plugin* create() {
    return new Adapter_I2C;
}

extern "C" void destroy(Plugin* p) {
	p->~Plugin();
    delete p;
}
#endif



