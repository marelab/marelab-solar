/*
 * DataLogger.h
 *
 *  Created on: 25.02.2014
 *      Author: marelab
 */

#ifndef DATALOGGER_H_
#define DATALOGGER_H_
#include <sqlite3.h>
#include <stdio.h>
#include <iostream>
#include <string>
namespace std {

class DataLogger {
private :
	int rc;
	sqlite3 *db;
	string sql;
	char *zErrMsg;


public:
	struct logentry {
	   char sensor_name[6];
	   long int timestamp;
	   float sensor_value;
	} entry;

	DataLogger(string dbpath);
	virtual ~DataLogger();

	void AddLogData(string name,long datum,float value);
	void Export(string filename,long date);


};

} /* namespace std */
#endif /* DATALOGGER_H_ */
