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

#ifndef Plugin_HPP_
#define Plugin_HPP_
#include <cstdio>
#include <string>
#include "json/json.h"
#include "IJsonSerializable.h"

using namespace std;

class Plugin : public IJsonSerializable{
protected:
     int adress;
     string typeOfPlugin;

public:
    Plugin(){}
    virtual ~Plugin(){}
    virtual string getName()=0;
    virtual string getVersion()=0;
    virtual string getTypeOfPlugin()=0;
    virtual Json::Value hardwareToUse()=0;


    virtual void Serialize( Json::Value& root ) = 0;
    virtual void Deserialize( Json::Value& root) = 0;
    virtual void SerializeAjax( Json::Value& root ) = 0;
    virtual void GetConfigAsJSON( string& configval) = 0;
    virtual void SetConfigAsJSON( Json::Value& para) = 0;


    // Functions each hardware Adapter has to offer
    // Getting Setting bool Values
    virtual bool getBoolState(string varName)=0;
    virtual void setBoolState(string varName,bool state)=0;

    virtual string getStringState(string varName)=0;
    virtual void   setStringState(string varName,string state)=0;

    virtual unsigned int getIntegerState(string varName)=0;
    virtual void   setIntegerState(string varName,unsigned int state)=0;

    virtual unsigned int* getArrayState(string varName)=0;
    virtual void   setArrayState(string varName,unsigned int* statearray)=0;

    virtual void work(tm calltime) = 0;

};

// the types of the class factories
typedef Plugin* create_t();
typedef void destroy_t(Plugin*);

#endif


