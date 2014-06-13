/*
 ============================================================================
 Name        : marelab-solar-deamon.c
 Author      :
 Version     :
 Copyright   : Marc Philipp Hammermann (c) 2012
 Description : Hello World in C, Ansi-style
 ============================================================================
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

      Json::StyledWriter writer;
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
