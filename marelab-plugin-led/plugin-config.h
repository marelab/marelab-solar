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

#ifndef PLUGIN_CONFIG_H_
#define PLUGIN_CONFIG_H_

#define pluginname 			"LedDimmer";
#define pluginversion 		"0.2";
#define TYPE_OF_PLUGIN  	"LOGIC";

struct ADAPTER_INFO {
    string AdapterName;
    string AdapterVersion;
};

// The Helper ADAPTER_COUNT = COUNT OF ADAPTERSS its needed because the size of STRUCT ARRAY can't
// be calculated!!!!!!!
int HELPER_ADAPTER_COUNT = 2;
ADAPTER_INFO COMPATIBLE_ADAPTERS[]  = {{"GPIO Adapter","0.1"},{"Modbus Adapter", "0.1"},{"I2C Adapter","0.1"}};


#endif /* PLUGIN_CONFIG_H_ */
