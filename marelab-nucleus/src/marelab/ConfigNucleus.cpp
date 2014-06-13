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



#include "ConfigNucleus.h"

string ConfigNucleus::getCfDaemonLockPath() const
{
    return cfDAEMON_LOCK_PATH;
}

void ConfigNucleus::setCfDaemonLockPath(string cfDaemonLockPath)
{
    cfDAEMON_LOCK_PATH = cfDaemonLockPath;
}

string ConfigNucleus::getCfDaemonName() const
{
    return cfDAEMON_NAME;
}

void ConfigNucleus::setCfDaemonName(string cfDaemonName)
{
    cfDAEMON_NAME = cfDaemonName;
}

string ConfigNucleus::getCfDaemonNameLock() const
{
    return cfDAEMON_NAME_LOCK;
}

void ConfigNucleus::setCfDaemonNameLock(string cfDaemonNameLock)
{
    cfDAEMON_NAME_LOCK = cfDaemonNameLock;
}


string ConfigNucleus::getCfDeamonPath() const
{
    return cfDEAMON_PATH;
}

void ConfigNucleus::setCfDeamonPath(string cfDeamonPath)
{
    cfDEAMON_PATH = cfDeamonPath;
}

string ConfigNucleus::getCfLogLevel() const
{
	return cfLOG_LEVEL;
}

void ConfigNucleus::setCfLogLevel(string loglevel)
{
  cfLOG_LEVEL = loglevel;
}

string ConfigNucleus::getCfNucleusVersion() const
{
    return cfNUCLEUS_VERSION;
}

void ConfigNucleus::setCfNucleusVersion(string cfNucleusVersion)
{
    cfNUCLEUS_VERSION = cfNucleusVersion;
}

string ConfigNucleus::getCfPlugindir() const
{
    return cfPLUGINDIR;
}

void ConfigNucleus::setCfPlugindir(string cfPlugindir)
{
    cfPLUGINDIR = cfPlugindir;
}

void ConfigNucleus::setCfDbPath(string cfDbPath){
	cfDB_PATH = cfDbPath;
}
string ConfigNucleus::getCfDbPath() const{
	return cfDB_PATH;
}

string ConfigNucleus::getCfRunAsUser() const
{
    return cfRUN_AS_USER;
}

void ConfigNucleus::setCfRunAsUser(string cfRunAsUser)
{
    cfRUN_AS_USER = cfRunAsUser;
}

string ConfigNucleus::getCfSockPath() const
{
    return cfSOCK_PATH;
}

void ConfigNucleus::setCfSockPath(string cfSockPath)
{
    cfSOCK_PATH = cfSockPath;
}



/* JSON Config File -> JavaObject*/
void ConfigNucleus::Deserialize( Json::Value& root )
{
	setCfDaemonLockPath(root["NUCLEUS"]["DAEMON_LOCK_PATH"].asString());
	setCfDaemonName(root["NUCLEUS"]["DAEMON_NAME"].asString());
	setCfDaemonNameLock(root["NUCLEUS"]["DAEMON_NAME_LOCK"].asString());
	setCfDeamonPath(root["NUCLEUS"]["DEAMON_PATH"].asString());
	setCfRunAsUser(root["NUCLEUS"]["RUN_AS_USER"].asString());
	setCfSockPath(root["NUCLEUS"]["SOCK_PATH"].asString());
	setCfPlugindir(root["NUCLEUS"]["PLUGINDIR"].asString());
	setCfNucleusVersion(root["NUCLEUS"]["NUCLEUS_VERSION"].asString());
	setCfLogLevel(root["NUCLEUS"]["LOG_LEVEL"].asString());
	setCfLogLevel(root["NUCLEUS"]["DB_PATH"].asString());

}
/**
 * ToDo Adding ConfigMarelab Obj -> configfile save
 */
void ConfigNucleus::SerializeAjax( Json::Value& root )
{

}

void ConfigNucleus::Serialize( Json::Value& root )
{
	root["NUCLEUS"]["DAEMON_LOCK_PATH"] = this->getCfDaemonLockPath();
	root["NUCLEUS"]["DAEMON_NAME"] = this->getCfDaemonName();
	root["NUCLEUS"]["DAEMON_NAME_LOCK"] = this->getCfDaemonNameLock();
	root["NUCLEUS"]["DEAMON_PATH"] = this->getCfDeamonPath();
	root["NUCLEUS"]["RUN_AS_USER"] = this->getCfRunAsUser();
	root["NUCLEUS"]["SOCK_PATH"] = this->getCfSockPath();
	root["NUCLEUS"]["PLUGINDIR"] = this->getCfPlugindir();
	root["NUCLEUS"]["NUCLEUS_VERSION"] = this->getCfNucleusVersion();
	root["NUCLEUS"]["LOG_LEVEL"] = this->getCfLogLevel();
	root["NUCLEUS"]["DB_PATH"] = this->getCfDbPath();
}

