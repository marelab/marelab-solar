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
 * Name        : ConfigMarelab.cpp
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



#include "ConfigMarelab.h"

string ConfigMarelab::getCfDaemonLockPath() const
{
    return cfDAEMON_LOCK_PATH;
}

void ConfigMarelab::setCfDaemonLockPath(string cfDaemonLockPath)
{
    cfDAEMON_LOCK_PATH = cfDaemonLockPath;
}

string ConfigMarelab::getCfDaemonName() const
{
    return cfDAEMON_NAME;
}

void ConfigMarelab::setCfDaemonName(string cfDaemonName)
{
    cfDAEMON_NAME = cfDaemonName;
}

string ConfigMarelab::getCfDaemonNameLock() const
{
    return cfDAEMON_NAME_LOCK;
}

void ConfigMarelab::setCfDaemonNameLock(string cfDaemonNameLock)
{
    cfDAEMON_NAME_LOCK = cfDaemonNameLock;
}


string ConfigMarelab::getCfDeamonPath() const
{
    return cfDEAMON_PATH;
}

void ConfigMarelab::setCfDeamonPath(string cfDeamonPath)
{
    cfDEAMON_PATH = cfDeamonPath;
}

string ConfigMarelab::getCfNucleusVersion() const
{
    return cfNUCLEUS_VERSION;
}

void ConfigMarelab::setCfNucleusVersion(string cfNucleusVersion)
{
    cfNUCLEUS_VERSION = cfNucleusVersion;
}

string ConfigMarelab::getCfPlugindir() const
{
    return cfPLUGINDIR;
}

void ConfigMarelab::setCfPlugindir(string cfPlugindir)
{
    cfPLUGINDIR = cfPlugindir;
}

string ConfigMarelab::getCfRunAsUser() const
{
    return cfRUN_AS_USER;
}

void ConfigMarelab::setCfRunAsUser(string cfRunAsUser)
{
    cfRUN_AS_USER = cfRunAsUser;
}

string ConfigMarelab::getCfSockPath() const
{
    return cfSOCK_PATH;
}

void ConfigMarelab::setCfSockPath(string cfSockPath)
{
    cfSOCK_PATH = cfSockPath;
}



/* JSON Config File -> JavaObject*/
void ConfigMarelab::Deserialize( Json::Value& root )
{
	setCfDaemonLockPath(root.get("DAEMON_LOCK_PATH", "").asString());
	setCfDaemonName(root.get("DAEMON_NAME", "").asString());
	setCfDaemonNameLock(root.get("DAEMON_NAME_LOCK", "").asString());
	setCfDeamonPath(root.get("DEAMON_PATH","").asString());
	setCfRunAsUser(root.get("RUN_AS_USER", "").asString());
	setCfSockPath(root.get("SOCK_PATH", "").asString());
	setCfPlugindir(root.get("PLUGINDIR", "").asString());
	setCfNucleusVersion(root.get("NUCLEUS_VERSION", "").asString());

}
/**
 * ToDo Adding ConfigMarelab Obj -> configfile save
 */
void ConfigMarelab::SerializeAjax( Json::Value& root )
{

}



void ConfigMarelab::Serialize( Json::Value& root )
{
	root["DAEMON_LOCK_PATH"] = this->getCfDaemonLockPath();
	root["DAEMON_NAME"] = this->getCfDaemonName();
	root["DAEMON_NAME_LOCK"] = this->getCfDaemonNameLock();
	root["DEAMON_PATH"] = this->getCfDeamonPath();
	root["RUN_AS_USER"] = this->getCfRunAsUser();
	root["SOCK_PATH"] = this->getCfSockPath();
	root["PLUGINDIR"] = this->getCfPlugindir();
	root["NUCLEUS_VERSION"] = this->getCfNucleusVersion();
}

