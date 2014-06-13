#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <stdint.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <time.h>
#include <fcntl.h>
#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/CgiUtils.h"
#include "cgicc/CgiEnvironment.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"
#include <iostream>
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>



#include <getopt.h>
//#define _GNU_SOURCE

using namespace std;
using namespace cgicc;

// The PiWeather board i2c address
char *DEVICE_BUS;
int ADRESS = 0x28;
int COMMAND = 0;
int PARA = 0;
int DEBUG = 0;
int val;
uint8_t i2c_command[10];
int i2c_comlen=0;
int i2c_OneCommad = 0;
int LOGGER = 0;
int CGICALL = 0;  // set to 1 if its called as CGI
// I2C Output Registers
union phI2C {
    float phwert;
    char b[sizeof(float)];
}PHI2C; 


// ANALYZER
FILE *fp;
char *line = NULL;
char json[255];
char test[255];
size_t len = 0;
ssize_t readlog;
long datum;
float ph;
long startdate = 0;
long enddate = 0;
int option = 0;
long hit = 0;
//

int cgimode =0; // Mode i2c ph read = 0 , log analyzer mode = 1;

// The I2C bus: This is for V2 pi's. For V1 Model B you need i2c-0
//static const char *devName = "/dev/i2c-1";





void print_usage() {
    printf(" usage  : marelab-phcgi -i [DEVICE  BUS] -a [adress] -c [command] (-p [para]) (-d)[DEBUG] (-l) logger\n");
    printf(" command: \n");
    printf("          1  -> Get PH\n");
    printf("          2  -> Set PH continuous read mode.\n");
    printf("          3  -> Set PH Poll Intervall para(1-255) sec.\n");
    printf("          8  -> Set PH to standby\n");
    printf("          9  -> Set PH Poll ON/OFF para(0-1)\n");
    printf("          x  -> Get PH Poll Returns Poll Intervall in sec.\n");
    printf(" For calibration: \n");
    printf("          1.) Insert probe into PH7 \n");
    printf("          2.) Send Command (2) set controller to PH (continuous read mode) \n");
    printf("          3.) Wait 2-3 min to stabilize the readed pH \n");
    printf("          4.) Send Command (7) Calibration ph 7 \n");
    printf("          5.) Clean & dry the probe \n");
    printf("          6.) Insert probe into PH4 \n");
    printf("          7.) Wait 2-3 min to stabilize the readed pH \n");
    printf("          8.) Send Command (4) Calibration ph 4 \n");
    printf("          9.) Clean & dry the probe \n");
    printf("         10.) Insert probe into PH10 \n");
    printf("         11.) Wait 2-3 min to stabilize the readed pH \n");
    printf("         12.) Send Command (10) Calibration ph 10 \n");
    printf("         13.) Send Command (8) set controller to PH (standby) \n");
    printf("              The probe is now ready to use") ;
    printf(" Calibration first insert PH probe into Suloution wait 2-3 min: \n");
    printf("          4  -> Calibration ph 4\n");
    printf("          7  -> Calibration ph 7\n");
    printf("          10 -> Calibration ph 10\n");
    printf(" optional: \n");
    printf("        -l activates that the output is added as JSON string to the log\n");
    printf("        -d aktivates debug console log\n");
    printf("\n");
    printf(" In analyzer Mode only these Parameters can be used\n");
    printf(" usage  : marelab-phcgi -s [startdate] -e [endate] (-d)[DEBUG]\n");
}

int main(int argc, char** argv) {
	int option = 0;

	while ((option = getopt(argc, argv, "i:a:c:p:s:e:dl")) != -1) {
			switch (option) {
			case 'i': {   // DEVICE_BUS selected
				DEVICE_BUS = optarg;
				cgimode = 0;
				break;
			}
			case 'a': {   // Adress selected
				ADRESS = atoi(optarg);
				cgimode = 0;
				break;
			}
			case 'c': {   // Command selected
				COMMAND = atoi(optarg);
				break;
			}
			case 'p': {   // Command selected
				PARA = atoi(optarg);
				break;
			}
			case 'd':
				DEBUG = 1;
				break;
			case 'l':
				LOGGER = 1;
				break;
			case 's': {   // Start Unix Date
				startdate = atol(optarg);
				cgimode = 1;
				break;
			}
			case 'e': {   // End Unix Date
				enddate = atol(optarg);
				cgimode = 1;
				break;
			}
			default:
				print_usage();
				exit(EXIT_FAILURE);
			}
		}

	try {
		Cgicc formData;
		const CgiEnvironment& cgienv = formData.getEnvironment();

		string scommand, sparameter;
		scommand = "";
		sparameter = "";

		string postdata = cgienv.getPostData();
		string encodedpostdata = cgicc::form_urldecode(postdata);
		if (postdata!=""){
			CGICALL = 1;
			//cout << "NON CGI CALL" << endl;
		syslog( LOG_ERR, "EncodedPostData: %s",encodedpostdata.c_str());

		/* Commando find*/
		form_iterator command = formData.getElement("COMMAND");
		if((command != formData.getElements().end()))
		{
			scommand = (**command).c_str();
			// Checken to start the Daemon, thats a specail command
			if (scommand=="PH")
			{
				syslog( LOG_ERR, "Geting PH value...");
			}
		}
		else
		{
			cout << "content-type: text/html" << endl << endl;
			cout << "CALL WITHOUT COMMAD" << 10 << endl;
		}

		/*Getting all form Parameters of the request and
		 * transforming it into a JSON string that
		 * the deamon can understand...
		 */
		vector <FormEntry> vf = formData.getElements();

			for (unsigned int c = 0; c < vf.size(); c++) {
				syslog(LOG_ERR, "POST ...[%s]-[%s]", vf[c].getName().c_str(),
						vf[c].getValue().c_str());
				if (vf[c].getName() == "START") {
					startdate = atol(vf[c].getValue().c_str());
					cgimode = 1;
				}
				if (vf[c].getName() == "ENDE") {
					enddate = atol(vf[c].getValue().c_str());
					cgimode = 1;
				}
			}

			//cout << "CGI: " << parameterString << endl;
		}
	}
	catch(const char *Exception) {
			cout << "content-type: text/html" << endl << endl;
			string errormsg = Exception;
			cout << "{\"ERROR\":\"the server seems to be down reason: ["+errormsg+"] check the logs for more info\"}";
			syslog( LOG_ERR, "the server seems to be down reason:  [%s] ... terminating",Exception );
	}




  if (DEBUG)
  	  printf("CGI MODE : %d\n", cgimode);

  	if (cgimode == 0) {
		if (DEBUG)
			printf("DEVICE_BUS : %s\n", DEVICE_BUS);
		if (DEBUG)
			printf("ADRESS : 0x%x\n", ADRESS);
		if (DEBUG)
			printf("COMMAND: %d\n", COMMAND);
		if (DEBUG)
			printf("PARA: %d\n", PARA);

		int file;

		if ((file = open(DEVICE_BUS, O_RDWR)) < 0) {
			fprintf(stderr, "<ERROR> I2C: Failed to access %s\n", DEVICE_BUS);
			exit(1);
		}

		if (DEBUG)
			printf("I2C: acquiring buss to 0x%x\n", ADRESS);

		if (ioctl(file, I2C_SLAVE, ADRESS) < 0) {
			fprintf(stderr,
					"<ERROR> I2C: Failed to acquire bus access/talk to slave 0x%x\n",
					ADRESS);
			exit(1);
		}

		if (COMMAND == 1) {     // Read PH from probe
			if (DEBUG)
				printf("I2C: get PH\n");
			i2c_command[0] = 1;    // Makes the ph controller read the pH
			i2c_command[1] = 10;   // Reads 1 byte of float;
			i2c_command[2] = 11;   // Reads 1 byte of float;
			i2c_command[3] = 12;   // Reads 1 byte of float;
			i2c_command[4] = 13;   // Reads 1 byte of float;
			i2c_comlen = 5;
			i2c_OneCommad = 1;
		}
		if (COMMAND == 2) {     // set to continous reading ph
			if (DEBUG)
				printf("I2C: Set continous reading\n");
			i2c_command[0] = 2;    // Command for Poll Intervall pH
			i2c_comlen = 1;
			i2c_OneCommad = 1;
		}
		if (COMMAND == 8) {     // set to continous reading ph
			if (DEBUG)
				printf("I2C: Set standby mode\n");
			i2c_command[0] = 8;    // Command for Poll Intervall pH
			i2c_comlen = 1;
			i2c_OneCommad = 1;
		}
		if (COMMAND == 3) {     // PH PollIntervall
			if (DEBUG)
				printf("I2C: Set Poll Intervall to %d sec\n", PARA);
			i2c_command[0] = 20;    // Command for Poll Intervall pH
			i2c_command[1] = PARA;  // Intervall in seconds
			i2c_comlen = 2;
			i2c_OneCommad = 0;
		}
		if (COMMAND == 9) {
			if (DEBUG)
				printf("I2C: PH Poll ON/OFF para %d\n", PARA);
			i2c_command[0] = 9;    // Makes the ph controller read the pH
			//i2c_command[1] = 10;   // Reads 1 byte of float;
			i2c_comlen = 1;
			i2c_OneCommad = 1;
		}
		if (COMMAND == 4) {
			if (DEBUG)
				printf("I2C: Calibration ph 4\n");
			i2c_command[0] = 4;    // Makes the ph controller read the pH
			i2c_comlen = 1;
			i2c_OneCommad = 1;
		}
		if (COMMAND == 7) {
			if (DEBUG)
				printf("I2C: Calibration ph 7\n");
			i2c_command[0] = 3;    // Makes the ph controller read the pH
			i2c_comlen = 1;
			i2c_OneCommad = 1;
		}
		if (COMMAND == 10) {
			if (DEBUG)
				printf("I2C: Calibration ph 10\n");
			i2c_command[0] = 5;    // Makes the ph controller read the pH
			i2c_comlen = 1;
			i2c_OneCommad = 1;
		}

		int pos;
		int bytes2write = 1;
		if (i2c_OneCommad == 0) {
			bytes2write = i2c_comlen;
			if (DEBUG)
				printf("Sending multiple I2C msgs\n");

		} else {
			if (DEBUG)
				printf("Sending One I2C msg\n");
			bytes2write = 1;
		}
		for (pos = 0; pos < i2c_comlen; pos++) {
			//uint8_t data[1];

			if (write(file, &i2c_command[pos], bytes2write) == 1) {
				if (DEBUG)
					printf("Sending %d\n", i2c_command[pos]);
				// As we are not talking to direct hardware but a microcontroller we
				// need to wait a short while so that it can respond.
				// 1ms seems to be enough but it depends on what workload it has
				usleep(10000);
				char buf[1];
				if ((COMMAND == 1) && (pos == 0)) {
					if (DEBUG)
						printf("Commando -> read ph\n");
					usleep(600000); // Keep a long delay that the ph controller gets the PH
				}
				if ((COMMAND == 1) && (pos > 0)) {
					if (read(file, buf, 1) == 1) {
						PHI2C.b[pos - 1] = buf[0];
						int temp = (int) buf[0];
						if (DEBUG)
							printf("Received %d\n", temp);
					}
				}
			}
			// Now wait else you could crash the arduino by sending requests too fast
			usleep(10000);
		}
		//}

		close(file);
		char logmsg[255];

		if (LOGGER == 1) {
			if (COMMAND == 1) {
				char str[30];
				char gtime[30];
				sprintf(gtime, "%d", time(NULL));
				sprintf(str, "%f", PHI2C.phwert);
				strcpy(logmsg, "{\"DATE\":");
				strcat(logmsg, gtime);
				strcat(logmsg, ",\"S\":\"PH1\",\"VALUE\":");
				strcat(logmsg, str);
				strcat(logmsg, "}");
				openlog("MARELAB_PH", LOG_PID | LOG_CONS, LOG_USER);
				syslog(LOG_INFO, logmsg);
				closelog();
			}
		}
		printf("%f", PHI2C.phwert);
		printf("<OK>\n");
	}
  	if (cgimode == 1) {
		if (DEBUG)
			printf("startdate : %d\n", startdate);
		if (DEBUG)
			printf("enddate: %d\n", enddate);

		fp = fopen("/var/log/marelab_ph.log", "r");
		if (fp == NULL)
			exit(EXIT_FAILURE);
		long count = 0;

		printf("content-type: application/json\n\n");

		printf("{\"PH\": [\n");
		while ((readlog = getline(&line, &len, fp)) != -1) {
			count++;
			if (count > 1)
				printf(",\n");
			sscanf(line, "%*s%*d%*d%*s%*s%*s%s", json);
			// get the Date from the JSON string
			sscanf(json, "{\"DATE\":%ld,\"S\":\"PH1\",\"VALUE\":%f%*s", &datum,
					&ph);
			//if ((datum >= startdate) && (datum  <= enddate))
			{
				hit++;
				if (DEBUG)
					printf("Analyse time:%ld ph: %f\n", datum, ph);
				printf("%s", json);
			}
		}
		printf("]}\n");

		if (DEBUG)
			printf("analyseph %d lines parsed %d json records exported\n",
					count, hit);
		free(line);
	}
	return (EXIT_SUCCESS);
}
