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

//#include "LedString.h"
//#include "LedTimerListe.h"
#include "IncomingMsg.h"
#include "../globals/IJsonSerializable.h"

IncomingMsg::IncomingMsg()
{

}
IncomingMsg::~IncomingMsg()
{

}

void IncomingMsg::SerializeAjax( Json::Value& root )
{

}

void IncomingMsg::Serialize( Json::Value& root )
{
}


Json::Value IncomingMsg::getCommand() const
{
    return command;
}

Json::Value IncomingMsg::getParameter() const
{
    return parameter;
}

Json::Value IncomingMsg::getParameter(string parametername) const
{
	if (parameter.isMember(parametername))
		return parameter.get("parametername","");
	else
	    return parameter.null;
}

void IncomingMsg::setCommand(Json::Value command)
{
    this->command = command;
}

void IncomingMsg::setParameter(Json::Value parameter)
{
    this->parameter = parameter;
}

void IncomingMsg::Deserialize( Json::Value& root )
{
	command = root.get("COMMAND","");
	parameter = root.get("PARAMETER","");
	//Json::Value para2;
	if (parameter.isString()){
		cout << "MSG PARA STRING:" << parameter.get("PARAMETER","").asString()<<endl;
	}
	else if (parameter.isObject()){
		//cout << "MSG PARA OBJ:" << parameter.toStyledString()<<endl;
		//cout << "MSG PARA OBJ:" << endl;
	}
	else if (parameter.isArray()){
		cout << "MSG PARA ARRAY:" << parameter.toStyledString() <<endl;
	}
	else{
		cout << "PARAMETER IS NO JSON" << endl;
	}
}
string IncomingMsg::getPlugin(){
	plugin = parameter.get("PLUGIN","").asString();
	return plugin;
}







