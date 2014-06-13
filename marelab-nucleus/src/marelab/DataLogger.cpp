/*
 * DataLogger.cpp
 *
 *  Created on: 25.02.2014
 *      Author: marelab
 */

#include "DataLogger.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <sqlite3.h>
#include "../json/json.h"

namespace std {


std::ofstream out("MarelabArchiev.txt");


static int callback(void *data, int argc, char **argv, char **azColName){
   int i;
   fprintf(stderr, "%s: ", (const char*)data);
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

static int callbackExport(void *data, int argc, char **argv, char **azColName){
   int i;
   string output;
   //fprintf(stderr, "%s: ", (const char*)data);
   Json::Value jObject;
   Json::StyledWriter writer;
   for(i=0; i<argc; i++){
      //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
      jObject[azColName[i]]= argv[i];
   }
   output = writer.write(jObject);
   out << output;
   return 0;
}

// Cleans Up the Database and exports all data older then 2 days
// to a archiev file
// filename : name of the file to export to
// daysold  : All data older then date - daysold gets exported
void DataLogger::Export(string filename,long date){
	string mydate;
	stringstream mystream;
	const char* data = "Callback function called";
	mystream << date;
	mydate = mystream.str();
	string sqlstatement = "select * from MARELABLOGGER where DATUM < " + mydate;


	/* Execute SQL statement */
	rc = sqlite3_exec(db, sqlstatement.c_str(), callbackExport,(void*) data, &zErrMsg);
	if( rc != SQLITE_OK ){
	   fprintf(stderr, "SQL error: %s\n", zErrMsg);
	   sqlite3_free(zErrMsg);
	}else{

	   fprintf(stdout, "Operation done successfully\n");
	}
	out.close();

	// Export DONE now DELETE THE ARCHIEVED ENTRIES & VACCUM THE DB FOR CLEANUP
	sqlstatement = "DELETE from MARELABLOGGER where DATUM < " + mydate+"; VACUUM MARELABLOGGER;";

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sqlstatement.c_str(), callback, (void*)data, &zErrMsg);
	if( rc != SQLITE_OK ){
	   fprintf(stderr, "SQL error: %s\n", zErrMsg);
	   sqlite3_free(zErrMsg);
	}else{
	   fprintf(stdout, "DELETE DB ENTRIES THAT ARE ARCHIEVED\n");
	}

	//

}
// Adding log events to the DB
void DataLogger::AddLogData(string name,long datum,float value){

	 	/* Create SQL statement */
	    string mydate,myvalue;
	    stringstream mystream,myval;
	    mystream << datum;
	    mydate = mystream.str();
	    myval << value;
	    myvalue = myval.str();

		string sqlstatement =
		    "INSERT INTO MARELABLOGGER (ID,NAME,DATUM,VALUE) VALUES (NULL,'"+ name + "',"+ mydate  + ","+ myvalue  + ");";
		//sprintf(query, "INSERT INTO MARELABLOGGER (ID,NAME,DATUM,VALUE) VALUES (NULL, '%s',"+datum+", "+value+");", (const char*)name.c_str());
	   //sql = "INSERT INTO MARELABLOGGER (ID,NAME,DATUM,VALUE) VALUES (NULL, '"+name.c_str()+"',"+datum+", "+value+"); " ;
	   /* Execute SQL statement */
	   rc = sqlite3_exec(db, sqlstatement.c_str(), callback, 0, &zErrMsg);
	   if( rc != SQLITE_OK ){
	      fprintf(stderr, "SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	   }else{
	      //fprintf(stdout, "Records created successfully\n");
	   }
}





DataLogger::DataLogger(string dbpath) {
	 zErrMsg = 0;

	 rc = sqlite3_open(dbpath.c_str(), &db);

	   if( rc ){
	      fprintf(stderr, "Can't open database: %s creating a new one!\n", sqlite3_errmsg(db));
	      exit (-1);
	   }

	      fprintf(stderr, "Opened database successfully\n");


	      /* Create SQL statement */
	        sql = "CREATE TABLE MARELABLOGGER("  \
	              "ID INTEGER PRIMARY KEY AUTOINCREMENT," \
	              "NAME           TEXT    NOT NULL," \
	              "DATUM          INT     NOT NULL," \
	              "VALUE          REAL    NOT NULL);";

	        /* Execute SQL statement */
	        rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
	        if( rc != SQLITE_OK ){
	        fprintf(stderr, "SQL msg: %s\n", zErrMsg);
	           sqlite3_free(zErrMsg);
	        }else{
	           fprintf(stdout, "Table created successfully\n");

	   }
}

DataLogger::~DataLogger() {
	// TODO Auto-generated destructor stub
	fprintf(stdout, "LOGGER DB CLOSED NORMAL\n");
	sqlite3_close(db);
}

} /* namespace std */
