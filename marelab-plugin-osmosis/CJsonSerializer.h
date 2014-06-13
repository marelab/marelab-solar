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

#ifndef CJSONSERIALIZER_H
#define CJSONSERIALIZER_H


#include <iostream>
#include <cstring>   // Required by strcpy()
#include <cstdlib>   // Required by malloc()
#include <vector>

#include "../json/json.h"
#include "IJsonSerializable.h"

#define TIMERSTORECOUNT 48


using namespace std;
/*
 * Timer stucts
 *
 */


class CJsonSerializer
{
public:
	~CJsonSerializer() {};

   static bool Serialize( IJsonSerializable* pObj, std::string& output )
   {
      if (pObj == NULL)
         return false;

      Json::Value serializeRoot;
      pObj->Serialize(serializeRoot);

      Json::StyledWriter writer;
      output = writer.write( serializeRoot );

      return true;
   };

   static bool SerializeAjax( IJsonSerializable* pObj, std::string& output )
   {
      if (pObj == NULL)
         return false;

      Json::Value serializeRoot;
      pObj->SerializeAjax(serializeRoot);

      Json::FastWriter writer;
      //Json::StyledWriter writer;
      output = writer.write( serializeRoot );

      return true;
   };

   static bool Deserialize( IJsonSerializable* pObj, std::string& input )
   {
      if (pObj == NULL)
         return false;

      Json::Value deserializeRoot;
      Json::Reader reader;

      if ( !reader.parse(input, deserializeRoot) )
         return false;

      pObj->Deserialize(deserializeRoot);

      return true;
   };
   CJsonSerializer( void ) {};

private:

};
#endif
