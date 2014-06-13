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
 * Name        : ConfigMarelab.h
 * Author      : Marc Philipp Hammermann
 * Version     :
 * Copyright © 2012 marc philipp hammermann  <marchammermann@googlemail.com>
 *
 * Description : Global Config class that holds main path & config vars
 *               these values a written as JSON Objects to the marelab.cfg
 *
 *
 *
 *
 */

#ifndef CONFIGMARELAB_H_
#define CONFIGMARELAB_H_


#include <iostream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include "../json/json.h"
#include "IJsonSerializable.h"

using namespace std;

class ConfigMarelab : public IJsonSerializable
{
private:

	 /* Objects to store in marelab.cfg */
	 string cfDAEMON_LOCK_PATH;			// Path for the lock pid
	 string cfDAEMON_NAME;				// Name of the marelab deamon shouldn't be changed
	 string cfDAEMON_NAME_LOCK;
	 string cfDEAMON_PATH;				// Path to the deamon executable
	 string cfRUN_AS_USER;				// User that runs the deamon
	 string cfSOCK_PATH;				// Socket File Path
	 string cfPLUGINDIR;				// Dir for nucleus plugins
	 string cfNUCLEUS_VERSION;			// Version of Nucleus


	Json::Value command;
	Json::Value parameter;

public:
	ConfigMarelab(){

	};
    ~ConfigMarelab(){

    };

    virtual void Serialize( Json::Value& root );
    virtual void Deserialize( Json::Value& root);
    virtual void SerializeAjax( Json::Value& root );

    string getCfDaemonLockPath() const;
    void setCfDaemonLockPath(string cfDaemonLockPath);
    string getCfDaemonName() const;
    void setCfDaemonName(string cfDaemonName);
    string getCfDaemonNameLock() const;
    void setCfDaemonNameLock(string cfDaemonNameLock);
    string getCfDeamonPath() const;
    void setCfDeamonPath(string cfDeamonPath);
    string getCfNucleusVersion() const;
    void setCfNucleusVersion(string cfNucleusVersion);
    string getCfPlugindir() const;
    void setCfPlugindir(string cfPlugindir);
    string getCfRunAsUser() const;
    void setCfRunAsUser(string cfRunAsUser);
    string getCfSockPath() const;
    void setCfSockPath(string cfSockPath);
    int getCfTransferBuffer() const;
    void setCfTransferBuffer(int cfTransferBuffer);

}
;

#endif /* CONFIGMARELAB_H_ */
